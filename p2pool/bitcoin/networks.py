import os
import platform

from twisted.internet import defer

from . import data
from p2pool.util import math, pack, jsonrpc

@defer.inlineCallbacks
def check_genesis_block(bitcoind, genesis_block_hash):
    try:
        yield bitcoind.rpc_getblock(genesis_block_hash)
    except jsonrpc.Error_for_code(-5):
        defer.returnValue(False)
    else:
        defer.returnValue(True)

nets = dict(
    memorycoin=math.Object(
        P2P_PREFIX='f9bbb6d9'.decode('hex'),
        P2P_PORT=1968,
        ADDRESS_VERSION=50,
        RPC_PORT=1925,
        RPC_CHECK=defer.inlineCallbacks(lambda bitcoind: defer.returnValue(
            (yield check_genesis_block(bitcoind, '0167f68b07587c231403069612646700deb086acc423a7b85f3304ac372e81d9')) and
            not (yield bitcoind.rpc_getinfo())['testnet']
        )),
        SUBSIDY_FUNC=lambda height: 280*100000000 * pow(0.95 ,(height + 1) // 1680), # Memorycoin has a 5% decline every 1680 blocks
        POW_FUNC=data.hash256,
        BLOCK_PERIOD=360, # s
        SYMBOL='MMC',
        CONF_FILE_FUNC=lambda: os.path.join(os.path.join(os.environ['APPDATA'], 'Memorycoin') if platform.system() == 'Windows' else os.path.expanduser('~/Library/Application Support/Memorycoin/') if platform.system() == 'Darwin' else os.path.expanduser('~/.memorycoin'), 'memorycoin.conf'),
        BLOCK_EXPLORER_URL_PREFIX='http://mmcexplorer.info/?query=',
        ADDRESS_EXPLORER_URL_PREFIX='http://mmcexplorer.info/?query=',
        TX_EXPLORER_URL_PREFIX='http://mmcexplorer.info/?query=',
        SANE_TARGET_RANGE=(2**256//4096 - 1, 2**256//2 - 1),
        DUMB_SCRYPT_DIFF=2**16,
        DUST_THRESHOLD=1e8,
    ),

)
for net_name, net in nets.iteritems():
    net.NAME = net_name

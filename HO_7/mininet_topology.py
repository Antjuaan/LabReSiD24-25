from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import Node
from mininet.log import setLogLevel, info
from mininet.cli import CLI
from mininet.link import TCLink

# Classe da passare come parametro al costruttore del Router
class GenericRouter(Node):
    def config(self, **params):
        super(GenericRouter, self).config(**params)

        # Enable forwarding on the router
        self.cmd('sysctl net.ipv4.ip_forward=1')
    
    def terminate(self):
        self.cmd('sysctl net.ipv4.ip_forward=0')
        super(GenericRouter, self).terminate()


# Classe per la topologia di rete
class MyTopo(Topo):
    def build(self):
        # Creazione del router
        router = self.addNode('r1', cls=GenericRouter, ip='10.0.1.254/24')

        # Creazione dei nodi host
        h1 = self.addHost('h1', ip='10.0.1.1/24', defaultRoute='via 10.0.1.254')
        h2 = self.addHost('h2', ip='10.0.2.1/24', defaultRoute='via 10.0.2.254')

        # Creazione delle interfacce di rete
        self.addLink(h1, router, intfName1='h1-eth0', intfName2='r1-eth0', params1={'ip': '10.0.1.1/24'}, params2={'ip': '10.0.1.254/24'},
                     cls=TCLink, bw=100, delay='75ms')
        self.addLink(h2, router, intfName1='h2-eth0', intfName2='r1-eth1', params1={'ip': '10.0.2.1/24'}, params2={'ip': '10.0.2.254/24'},
                     cls=TCLink, bw=100, delay='75ms')


# Funzione principale per eseguire la topologia
def main():
    # Istanza della topologia
    topo = MyTopo()

    # Creazione della rete
    net = Mininet(topo=topo, controller=None)

    # Avvio della rete
    net.start()

    # Avvio la CLI per fare test
    CLI(net)

    # Terminazione della rete
    net.stop()

if __name__ == '__main__':
    main()
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Exercise7");

int
main(int argc, char *argv[])
{
  LogComponentEnable("UdpReliableEchoClientApplication", (LogLevel)(LOG_LEVEL_INFO|LOG_PREFIX_TIME|LOG_PREFIX_NODE));
  LogComponentEnable("UdpReliableEchoServerApplication", (LogLevel)(LOG_LEVEL_INFO|LOG_PREFIX_TIME|LOG_PREFIX_NODE));

  double simulationTime = 30.0;

  NodeContainer nodes;
  nodes.Create(4);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("5ms"));
  /*Set Network Queue Size --> set small queue for packet losses*/
  //pointToPoint.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue ("1p")); //Queue size : 1 packet
  pointToPoint.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue ("1500B")); //Queue size : 1500 bytes == about 1 packet


  NetDeviceContainer devicesA1toB;
  devicesA1toB = pointToPoint.Install(nodes.Get(0), nodes.Get(2));

  NetDeviceContainer devicesA2toB;
  devicesA2toB = pointToPoint.Install(nodes.Get(1), nodes.Get(2));

  NetDeviceContainer devicesBtoC;
  devicesBtoC = pointToPoint.Install(nodes.Get(2), nodes.Get(3));

  InternetStackHelper stack;
  stack.Install(nodes);

  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesA1toB = address.Assign(devicesA1toB);

  address.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesA2toB = address.Assign(devicesA2toB);

  address.SetBase("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesBtoC = address.Assign(devicesBtoC);

  // Set up the routing tables --->> Important for multiple hop transmission
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  UdpReliableEchoClientHelper echoClient(interfacesBtoC.GetAddress(1), 9);
  echoClient.SetAttribute("MaxPackets", UintegerValue(1000000));
  echoClient.SetAttribute("PacketSize", UintegerValue(1024));
  //Set high send rate for packet losses
  echoClient.SetAttribute("Interval", TimeValue(Seconds(0.001))); // 1024 * 8 * 0.001 = 8 Mbps > 1 Mbps

  ApplicationContainer clientApps;
  clientApps.Add(echoClient.Install(nodes.Get(0)));

  echoClient.SetAttribute("RemotePort", UintegerValue(10));
  clientApps.Add(echoClient.Install(nodes.Get(1)));

  clientApps.Start(Seconds(1.0));
  clientApps.Stop(Seconds(simulationTime));

  UdpReliableEchoServerHelper echoServer(9);
  ApplicationContainer serverApps(echoServer.Install(nodes.Get(3)));

  echoServer.SetAttribute("Port", UintegerValue(10));
  serverApps.Add(echoServer.Install(nodes.Get(3)));

  serverApps.Start(Seconds(0));
  serverApps.Stop(Seconds(simulationTime + 1.0));

  Simulator::Run();
  Simulator::Stop(Seconds(simulationTime + 3.0));

  Simulator::Destroy();
  return 0;
}

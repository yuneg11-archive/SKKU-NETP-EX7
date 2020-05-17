waf_path="../waf"

export "NS_LOG=Exercise7=level_info:UdpReliableEchoClientApplication=level_info:UdpReliableEchoServerApplication=level_info"
${waf_path} --run Exercise7 2> log.out
grep "s 0 " log.out > log0.out
grep "s 1 " log.out > log1.out
grep "s 3 " log.out > log3.out
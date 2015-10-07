package main

import (
	"bufio"
	"encoding/binary"
	"fmt"
	"github.com/codegangsta/cli"
	//	"io/ioutil"
	"net"
	"os"
)

const (
	CONTROLLER         = "192.168.123.201"
	PORT               = "2020"
	CONTROL_TAG        = 3
	OUT_TAG            = 2
	TAG_ADD            = 1
	TAG_DEL            = 2
	TAG_DUMP           = 3
	TAG_CLEAR          = 4
	OUTTAG_ADD         = 5
	OUTTAG_DEL         = 6
	OUTTAG_DUMP        = 7
	OUTTAG_CLEAR       = 8
	MB_ADD             = 9
	MB_DEL             = 10
	MB_DUMP            = 11
	MB_CLEAR           = 12
	FW_ADD             = 13
	FW_DEL             = 14
	FW_DUMP            = 15
	FW_CLEAR           = 16
	TEST_ADD           = 17
	TEST_DEL           = 18
	TAG_GENERATE       = 19
	TAG_GENERATE_DEL   = 20
	TAG_GENERATE_CLEAR = 21
)

type FTHeader struct {
	messageType int32
	mbid        int32
	tag         int32
	state       int32
}

func NewFTHeader(messageType, mbid, tag, state int32) *FTHeader {
	self := new(FTHeader)
	self.messageType = messageType
	self.mbid = mbid
	self.tag = tag
	self.state = state
	return self
}

func (self *FTHeader) bytes() []byte {
	bytes := make([]byte, 0, 1024)
	bytes = append(bytes, createBytes(self.messageType)...)
	bytes = append(bytes, createBytes(self.mbid)...)
	bytes = append(bytes, createBytes(self.tag)...)
	bytes = append(bytes, createBytes(self.state)...)
	return bytes
}

type FTControl struct {
	controlType int
	args        []byte
}

func NewFTControl(controlType int) *FTControl {
	self := new(FTControl)
	self.controlType = controlType
	self.args = make([]byte, 0, 512)
	return self
}

func (self *FTControl) addArgWithBytes(arg []byte) {
	self.args = append(self.args, arg...)
}

func (self *FTControl) addArgWithInt(arg int) {
	self.args = append(self.args, createBytes(int32(arg))...)
}

func (self *FTControl) addArgWithInt64(arg int64) {
	self.args = append(self.args, createBytes64(int64(arg))...)
}

func (self *FTControl) bytes() []byte {
	bytes := make([]byte, 0, 1024)
	bytes = append(bytes, createBytes(int32(self.controlType))...)
	bytes = append(bytes, self.args...)
	return bytes
}

type FTControlMessage struct {
	header  *FTHeader
	message *FTControl
}

func NewFTControlMessage(controlType int) *FTControlMessage {
	self := new(FTControlMessage)
	self.header = NewFTHeader(CONTROL_TAG, 0, 0, 0)
	self.message = NewFTControl(controlType)
	return self
}

func NewFTConsumeMessage(mbid, state, tag int) *FTControlMessage {
	self := new(FTControlMessage)
	self.header = NewFTHeader(OUTTAG_ADD, int32(mbid), int32(state), int32(tag))
	return self
}

func (self *FTControlMessage) bytes() []byte {
	bytes := make([]byte, 0, 1024)
	bytes = append(bytes, self.header.bytes()...)
	bytes = append(bytes, self.message.bytes()...)
	return bytes
}

func createBytes(m int32) []byte {
	bytes := make([]byte, 4)
	binary.BigEndian.PutUint32(bytes, uint32(m))
	return bytes
}

func createBytes64(m int64) []byte {
	bytes := make([]byte, 8)
	binary.BigEndian.PutUint64(bytes, uint64(m))
	return bytes
}

func pushMessage(ftMessage *FTControlMessage) {
	conn, err := net.Dial("tcp", CONTROLLER+":"+PORT)
	defer conn.Close()
	if err != nil {
		panic(err)
	}
	_, err = conn.Write(ftMessage.bytes())
	reader := bufio.NewReader(conn)
	reply := make([]byte, 0, 1024)
	for {
		tmpResult, err := reader.ReadBytes('\n')
		if err != nil {
			panic(err)
		}
		if len(tmpResult) == 1 {
			break
		}
		reply = append(reply, tmpResult...)
	}
	fmt.Println(string(reply[16:]))
}

func main() {
	app := cli.NewApp()
	app.Name = "Control App for Flowtag controller"
	app.EnableBashCompletion = true
	app.Commands = []cli.Command{
		{
			Name:  "tag",
			Usage: "Configure Tag DB",
			Subcommands: []cli.Command{
				{
					Name:  "add",
					Usage: "Add tag into tag DB",
					Flags: []cli.Flag{
						cli.IntFlag{Name: "tag", Value: -1, Usage: "tag"},
						cli.StringFlag{Name: "srcIP", Value: "", Usage: "src IP"},
						cli.StringFlag{Name: "dstIP", Value: "", Usage: "dsdt IP"},
						cli.IntFlag{Name: "srcPort", Value: 0, Usage: "src Port"},
						cli.IntFlag{Name: "dstPort", Value: 0, Usage: "dst Port"},
						cli.StringFlag{Name: "proto", Value: "", Usage: "Protocol <ICMP|UDP|TCP>"},
						cli.IntFlag{Name: "next", Value: 5, Usage: "next port"},
					},
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(TAG_ADD)

						if c.Int("tag") == -1 {
							fmt.Println("Tag is required")
						}
						controlMessage.message.addArgWithInt(c.Int("tag"))

						if c.String("srcIP") != "" {
							srcIP := net.ParseIP(c.String("srcIP"))
							if srcIP == nil {
								panic("srcIP cannot be parsed")
							}
							srcIPBytes := []byte(srcIP)[12:]
							controlMessage.message.addArgWithBytes(srcIPBytes)
						} else {
							controlMessage.message.addArgWithInt(0)
						}

						if c.String("dstIP") != "" {
							dstIP := net.ParseIP(c.String("dstIP"))
							if dstIP == nil {
								panic("dstIP cannot be parsed")
							}
							dstIPBytes := []byte(dstIP)[12:]
							controlMessage.message.addArgWithBytes(dstIPBytes)
						} else {
							controlMessage.message.addArgWithInt(0)
						}

						controlMessage.message.addArgWithInt(c.Int("srcPort"))
						controlMessage.message.addArgWithInt(c.Int("dstPort"))

						proto := 0
						if c.String("proto") != "" {
							switch c.String("proto") {
							case "icmp", "ICMP":
								proto = 1
							case "udp", "UDP":
								proto = 17
							case "tcp", "TCP":
								proto = 6
							}
						}
						controlMessage.message.addArgWithInt(proto)
						controlMessage.message.addArgWithInt(c.Int("next"))
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "del",
					Usage: "Delete tag from tag DB",
					Flags: []cli.Flag{
						cli.IntFlag{Name: "tag", Value: 0, Usage: "tag"},
					},
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(TAG_DEL)
						if c.Int("tag") == 0 {
							fmt.Println("Tag is required")
						}
						controlMessage.message.addArgWithInt(c.Int("tag"))
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "dump",
					Usage: "Dump tag DB",
					Action: func(c *cli.Context) {
						if len(c.Args()) != 0 {
							fmt.Println("Invalid Usage:tag dump")
							return
						}
						controlMessage := NewFTControlMessage(TAG_DUMP)
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "clear",
					Usage: "Clear tag DB",
					Action: func(c *cli.Context) {
						if len(c.Args()) != 0 {
							fmt.Println("Invalid Usage:tag clear")
							return
						}
						controlMessage := NewFTControlMessage(TAG_CLEAR)
						pushMessage(controlMessage)
					},
				},
			},
		},
		{
			Name:  "out",
			Usage: "Configure Out Tag DB",
			Subcommands: []cli.Command{
				{
					Name:  "add",
					Usage: "Add out tag DB",
					Flags: []cli.Flag{
						cli.IntFlag{Name: "mbId", Value: -1, Usage: "middle-box id"},
						cli.IntFlag{Name: "state", Value: -1, Usage: "middle-box state"},
						cli.IntFlag{Name: "preTag", Value: -1, Usage: "preTag"},
						cli.IntFlag{Name: "newTag", Value: -1, Usage: "newTag"},
						cli.IntFlag{Name: "next", Value: -1, Usage: "nextHost"},
					},
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(OUTTAG_ADD)

						if c.Int("mbId") == -1 || c.Int("state") == -1 || c.Int("preTag") == -1 || c.Int("newTag") == -1 || c.Int("next") == -1 {
							fmt.Println("Invalid Usage: out add <tag> <state> <preTag> <newTag>")
							return
						}
						controlMessage.message.addArgWithInt(c.Int("mbId"))
						controlMessage.message.addArgWithInt(c.Int("state"))
						controlMessage.message.addArgWithInt(c.Int("preTag"))
						controlMessage.message.addArgWithInt(c.Int("newTag"))
						controlMessage.message.addArgWithInt(c.Int("next"))
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "del",
					Usage: "Delete from out tag DB",
					Flags: []cli.Flag{
						cli.IntFlag{Name: "mbId", Value: -1, Usage: "middle-box id"},
						cli.IntFlag{Name: "state", Value: -1, Usage: "middle-box state"},
						cli.IntFlag{Name: "preTag", Value: -1, Usage: "preTag"},
					},
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(OUTTAG_DEL)

						if c.Int("mbId") == -1 || c.Int("state") == -1 || c.Int("preTag") == -1 {
							fmt.Println("Invalid Usage: out del <tag> <state> <preTag>")
							return
						}
						controlMessage.message.addArgWithInt(c.Int("mbId"))
						controlMessage.message.addArgWithInt(c.Int("state"))
						controlMessage.message.addArgWithInt(c.Int("preTag"))
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "dump",
					Usage: "Dump out tag DB",
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(OUTTAG_DUMP)

						if len(c.Args()) != 0 {
							fmt.Println("Invalid Usage: out dump")
							return
						}
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "clear",
					Usage: "Clear out tag DB",
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(OUTTAG_CLEAR)

						if len(c.Args()) != 0 {
							fmt.Println("Invalid Usage: out clear")
							return
						}
						pushMessage(controlMessage)
					},
				},
			},
		},
		{
			Name:  "mb",
			Usage: "Configure MiddleBox DB",
			Subcommands: []cli.Command{
				{
					Name:  "add",
					Usage: "Add MiddleBox DB",
					Flags: []cli.Flag{
						cli.IntFlag{Name: "mbId", Value: -1, Usage: "middle-box id"},
						cli.IntFlag{Name: "hostId", Value: -1, Usage: "host id/location"},
						cli.StringFlag{Name: "type", Value: "", Usage: "Middle-box type <NAT|CONSUME|NON_CONSUME|MONITOR>"},
						cli.StringFlag{Name: "address", Value: "", Usage: "MiddleBox Address"},
						cli.StringFlag{Name: "mask", Value: "255.255.255.255", Usage: "Middlebox Address Mask"},
					},
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(MB_ADD)
						if c.Int("mbId") == -1 || c.Int("hostId") == -1 || c.String("type") == "" {
							fmt.Println("Invalid Usage: mb add <mbId> <hostId> <type>")
							return
						}
						controlMessage.message.addArgWithInt(c.Int("mbId"))
						controlMessage.message.addArgWithInt(c.Int("hostId"))
						var mbType int
						switch c.String("type") {
						case "NAT":
							mbType = 0
						case "CONSUME":
							mbType = 1
						case "NON_CONSUME":
							mbType = 2
						case "MONITOR":
							mbType = 3
						default:
							mbType = -1
							return
						}
						controlMessage.message.addArgWithInt(mbType)
						if c.String("address") != "" {
							ip := net.ParseIP(c.String("address"))
							if ip == nil {
								panic("ip cannot be parsed")
							}
							ipBytes := []byte(ip)[12:]
							mask := net.ParseIP(c.String("mask"))
							if mask == nil {
								panic("mask cannot be parsed")
							}
							maskBytes := []byte(mask)[12:]
							for index, _ := range ipBytes {
								ipBytes[index] &= maskBytes[index]
							}
							controlMessage.message.addArgWithBytes(ipBytes)
							controlMessage.message.addArgWithBytes(maskBytes)
						} else {
							controlMessage.message.addArgWithInt(0)
							controlMessage.message.addArgWithInt(0)
						}
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "dump",
					Usage: "Dump MB DB",
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(MB_DUMP)

						if len(c.Args()) != 0 {
							fmt.Println("Invalid Usage: mb dump")
							return
						}
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "clear",
					Usage: "Clear mb tag DB",
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(MB_CLEAR)

						if len(c.Args()) != 0 {
							fmt.Println("Invalid Usage: mb clear")
							return
						}
						pushMessage(controlMessage)
					},
				},
			},
		},
		{
			Name:  "fw",
			Usage: "Configure Forwarding Table for CoreRouter",
			Subcommands: []cli.Command{
				{
					Name:  "add",
					Usage: "Add Forwading Table",
					Flags: []cli.Flag{
						cli.IntFlag{Name: "swId", Value: -1, Usage: "sw id"},
						cli.IntFlag{Name: "hostId", Value: -1, Usage: "mb id"},
						cli.IntFlag{Name: "connectorId", Value: -1, Usage: "connector ID"},
					},
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(FW_ADD)
						if c.Int("swId") == -1 || c.Int("hostId") == -1 || c.Int("connectorId") == -1 {
							fmt.Println("Invalid Usage: fw add <swid> <hostid> <connectorId>")
							return
						}
						controlMessage.message.addArgWithInt64(int64(c.Int("swId")))
						controlMessage.message.addArgWithInt(c.Int("hostId"))
						controlMessage.message.addArgWithInt(c.Int("connectorId"))
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "del",
					Usage: "Dell Forwading Table",
					Flags: []cli.Flag{
						cli.IntFlag{Name: "swId", Value: -1, Usage: "sw id"},
						cli.IntFlag{Name: "hostId", Value: -1, Usage: "mb id"},
					},
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(FW_DEL)
						if c.Int("swId") == -1 || c.Int("hostId") == -1 {
							fmt.Println("Invalid Usage: fw del <swid> <hostId>")
							return
						}
						controlMessage.message.addArgWithInt64(int64(c.Int("swId")))
						controlMessage.message.addArgWithInt(c.Int("hostId"))
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "dump",
					Usage: "Dump Fowarding Table",
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(FW_DUMP)

						if len(c.Args()) != 0 {
							fmt.Println("Invalid Usage: fw dump")
							return
						}
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "clear",
					Usage: "Clear Forwarding Table",
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(FW_CLEAR)
						if len(c.Args()) != 0 {
							fmt.Println("Invalid Usage: mb clear")
							return
						}
						pushMessage(controlMessage)
					},
				},
			},
		},
		{
			Name:  "test",
			Usage: "Configure Test",
			Subcommands: []cli.Command{
				{
					Name:  "add",
					Usage: "Add Forwading Table",
					Flags: []cli.Flag{
						cli.IntFlag{Name: "tag", Value: -1, Usage: "tag id"},
						cli.IntFlag{Name: "generator", Value: -1, Usage: "generator host id"},
						cli.IntFlag{Name: "input", Value: -1, Usage: "input host id"},
						cli.StringFlag{Name: "generatorIP", Value: "", Usage: "mb id"},
						cli.StringFlag{Name: "inputIP", Value: "", Usage: "connector ID"},
					},
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(TEST_ADD)
						if c.Int("tag") == -1 || c.Int("generator") == -1 || c.Int("input") == -1 {
							fmt.Println("Invalid Usage:")
							return
						}
						controlMessage.message.addArgWithInt(c.Int("tag"))
						controlMessage.message.addArgWithInt(c.Int("generator"))
						controlMessage.message.addArgWithInt(c.Int("input"))
						if c.String("generatorIP") != "" {
							ip := net.ParseIP(c.String("generatorIP"))
							if ip == nil {
								panic("generatorIP cannot be parsed")
							}
							ipBytes := []byte(ip)[12:]
							controlMessage.message.addArgWithBytes(ipBytes)
						} else {
							controlMessage.message.addArgWithInt(0)
						}
						if c.String("inputIP") != "" {
							ip := net.ParseIP(c.String("inputIP"))
							if ip == nil {
								panic("generatorIP cannot be parsed")
							}
							ipBytes := []byte(ip)[12:]
							controlMessage.message.addArgWithBytes(ipBytes)
						} else {
							controlMessage.message.addArgWithInt(0)
						}
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "del",
					Usage: "Dell Forwading Table",
					Flags: []cli.Flag{
						cli.IntFlag{Name: "swId", Value: -1, Usage: "sw id"},
						cli.IntFlag{Name: "hostId", Value: -1, Usage: "mb id"},
					},
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(FW_DEL)
						if c.Int("swId") == -1 || c.Int("hostId") == -1 {
							fmt.Println("Invalid Usage: fw del <swid> <hostId>")
							return
						}
						controlMessage.message.addArgWithInt64(int64(c.Int("swId")))
						controlMessage.message.addArgWithInt(c.Int("hostId"))
						pushMessage(controlMessage)
					},
				},
			},
		},
		{
			Name:  "rule",
			Usage: "explicit rule",
			Subcommands: []cli.Command{
				{
					Name:  "add",
					Usage: "Add Forwading Table",
					Flags: []cli.Flag{
						cli.IntFlag{Name: "hostId", Value: -1, Usage: "hostId"},
						cli.IntFlag{Name: "tag", Value: -1, Usage: "tag"},
						cli.StringFlag{Name: "srcIP", Value: "", Usage: "src IP"},
						cli.StringFlag{Name: "dstIP", Value: "", Usage: "dst IP"},
						cli.IntFlag{Name: "srcPort", Value: -1, Usage: "src Port"},
						cli.IntFlag{Name: "dstPort", Value: -1, Usage: "dst Port"},
						cli.StringFlag{Name: "proto", Value: "", Usage: "protocol"},
					},
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(TAG_GENERATE)
						if c.Int("hostId") == -1 || c.Int("tag") == -1 {
							fmt.Println("Invalid Usage:")
							return
						}
						controlMessage.message.addArgWithInt(c.Int("hostId"))
						controlMessage.message.addArgWithInt(c.Int("tag"))
						if c.String("srcIP") != "" {
							ip := net.ParseIP(c.String("srcIP"))
							if ip == nil {
								panic("src IP cannot be parsed")
							}
							ipBytes := []byte(ip)[12:]
							controlMessage.message.addArgWithBytes(ipBytes)
						} else {
							controlMessage.message.addArgWithInt(0)
						}
						if c.String("dstIP") != "" {
							ip := net.ParseIP(c.String("dstIP"))
							if ip == nil {
								panic("dstIP cannot be parsed")
							}
							ipBytes := []byte(ip)[12:]
							controlMessage.message.addArgWithBytes(ipBytes)
						} else {
							controlMessage.message.addArgWithInt(0)
						}
						controlMessage.message.addArgWithInt(c.Int("srcPort"))
						controlMessage.message.addArgWithInt(c.Int("dstPort"))
						proto := 0
						if c.String("proto") != "" {
							switch c.String("proto") {
							case "icmp", "ICMP":
								proto = 1
							case "udp", "UDP":
								proto = 17
							case "tcp", "TCP":
								proto = 6
							}
						}
						controlMessage.message.addArgWithInt(proto)
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "del",
					Usage: "Dell Rule",
					Flags: []cli.Flag{
						cli.IntFlag{Name: "hostId", Value: -1, Usage: "hostId"},
						cli.IntFlag{Name: "tag", Value: -1, Usage: "tag"},
						cli.StringFlag{Name: "srcIP", Value: "", Usage: "src IP"},
						cli.StringFlag{Name: "dstIP", Value: "", Usage: "dst IP"},
					},
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(TAG_GENERATE_DEL)
						if c.Int("hostId") == -1 || c.Int("tag") == -1 || c.String("srcIP") == "" || c.String("dstIP") == "" {
							fmt.Println("Invalid Usage:")
							return
						}
						controlMessage.message.addArgWithInt(c.Int("hostId"))
						controlMessage.message.addArgWithInt(c.Int("tag"))
						if c.String("srcIP") != "" {
							ip := net.ParseIP(c.String("srcIP"))
							if ip == nil {
								panic("src IP cannot be parsed")
							}
							ipBytes := []byte(ip)[12:]
							controlMessage.message.addArgWithBytes(ipBytes)
						} else {
							controlMessage.message.addArgWithInt(0)
						}
						if c.String("dstIP") != "" {
							ip := net.ParseIP(c.String("dstIP"))
							if ip == nil {
								panic("dstIP cannot be parsed")
							}
							ipBytes := []byte(ip)[12:]
							controlMessage.message.addArgWithBytes(ipBytes)
						} else {
							controlMessage.message.addArgWithInt(0)
						}
						pushMessage(controlMessage)
					},
				},
				{
					Name:  "clear",
					Usage: "Clear Rule",
					Flags: []cli.Flag{
						cli.IntFlag{Name: "hostId", Value: -1, Usage: "hostId"},
					},
					Action: func(c *cli.Context) {
						controlMessage := NewFTControlMessage(TAG_GENERATE_DEL)
						if c.Int("hostId") == -1 {
							fmt.Println("Invalid Usage:")
							return
						}
						controlMessage.message.addArgWithInt(c.Int("hostId"))
						pushMessage(controlMessage)
					},
				},
			},
		},
	}
	app.Run(os.Args)
}

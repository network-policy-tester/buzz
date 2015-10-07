package main

import (
	"bufio"
	"encoding/binary"
	"fmt"
	//"github.com/codegangsta/cli"
	//  "io/ioutil"
	"net"
	//"os"
)

const (
	CONTROLLER  = "192.168.123.201"
	PORT        = "2020"
	CONTROL_TAG = 3
	OUT_TAG     = 2
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

func createBytes(m int32) []byte {
	bytes := make([]byte, 4)
	binary.BigEndian.PutUint32(bytes, uint32(m))
	return bytes
}

func (self *FTHeader) bytes() []byte {
	bytes := make([]byte, 0, 1024)
	bytes = append(bytes, createBytes(self.messageType)...)
	bytes = append(bytes, createBytes(self.mbid)...)
	bytes = append(bytes, createBytes(self.tag)...)
	bytes = append(bytes, createBytes(self.state)...)
	return bytes
}

type FTControlMessage struct {
	header *FTHeader
}

func NewFTConsumeMessage(mbid, state, tag int) *FTControlMessage {
	self := new(FTControlMessage)
	self.header = NewFTHeader(OUT_TAG, int32(mbid), int32(state), int32(tag))
	return self
}

func (self *FTControlMessage) bytes() []byte {
	bytes := make([]byte, 0, 1024)
	bytes = append(bytes, self.header.bytes()...)
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
	fmt.Println(reply)
}

func main() {
	message := NewFTConsumeMessage(61, 0, 0)
	pushMessage(message)
}

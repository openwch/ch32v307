/*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "StackTrace.h"
#include "MQTTPacket.h"
#include <string.h>

/**
  * Returns the number of bytes the "remaining length" field occupies, or -1
  * if the field is malformed or does not fit into the supplied data.
  * The field is at most 4 bytes long and its last byte has the top bit cleared.
  * @param buf the buffer holding the "remaining length" field
  * @param buflen the number of bytes available in buf
  * @return the field length in bytes, or -1 on failure
  */
static int remainingLengthLen(const unsigned char* buf, int buflen)
{
	int i;

	for (i = 0; (i < buflen) && (i < 4); i++)
	{
		if ((buf[i] & 0x80) == 0)
			return i + 1;
	}
	return -1;
}

/**
  * Deserializes the supplied (wire) buffer into publish data
  * @param dup returned integer - the MQTT dup flag
  * @param qos returned integer - the MQTT QoS value
  * @param retained returned integer - the MQTT retained flag
  * @param packetid returned integer - the MQTT packet identifier
  * @param topicName returned MQTTString - the MQTT topic in the publish
  * @param payload returned byte buffer - the MQTT publish payload
  * @param payloadlen returned integer - the length of the MQTT payload
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buflen the length in bytes of the data in the supplied buffer
  * @return error code.  1 is success
  */
int MQTTDeserialize_publish(unsigned char* dup, int* qos, unsigned char* retained, unsigned short* packetid, MQTTString* topicName,
		unsigned char** payload, int* payloadlen, unsigned char* buf, int buflen)
{
	MQTTHeader header = {0};
	unsigned char* curdata = buf;
	unsigned char* enddata = NULL;
	unsigned char* hardend = buf + buflen; /* first byte past the supplied data */
	int rc = 0;
	int mylen = 0;
	int remlenbytes = 0;

	FUNC_ENTRY;
	if (buflen < 2) /* header byte plus at least one remaining length byte */
		goto exit;
	header.byte = readChar(&curdata);
	if (header.bits.type != PUBLISH)
		goto exit;
	*dup = header.bits.dup;
	*qos = header.bits.qos;
	*retained = header.bits.retain;

	/* the remaining length field is read only after it has been shown to end
	 * inside the supplied data, so that decoding it cannot run past hardend */
	remlenbytes = remainingLengthLen(curdata, (int)(hardend - curdata));
	if (remlenbytes < 0)
		goto exit;
	curdata += (rc = MQTTPacket_decodeBuf(curdata, &mylen)); /* read remaining length */
	if ((rc != remlenbytes) || (mylen < 0))
		goto exit;

	/* mylen is chosen by the peer, so it must not be allowed to reach past the
	 * bytes that were really received */
	if (curdata + mylen > hardend)
		goto exit;
	enddata = curdata + mylen;

	if (!readMQTTLenString(topicName, &curdata, enddata) ||
		enddata - curdata < 0) /* do we have enough data to read the protocol version byte? */
		goto exit;

	if (*qos > 0)
	{
		if (enddata - curdata < 2) /* enough data for the packet identifier? */
			goto exit;
		*packetid = readInt(&curdata);
	}

	*payloadlen = enddata - curdata;
	*payload = curdata;
	rc = 1;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}



/**
  * Deserializes the supplied (wire) buffer into an ack
  * @param packettype returned integer - the MQTT packet type
  * @param dup returned integer - the MQTT dup flag
  * @param packetid returned integer - the MQTT packet identifier
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buflen the length in bytes of the data in the supplied buffer
  * @return error code.  1 is success, 0 is failure
  */
int MQTTDeserialize_ack(unsigned char* packettype, unsigned char* dup, unsigned short* packetid, unsigned char* buf, int buflen)
{
	MQTTHeader header = {0};
	unsigned char* curdata = buf;
	unsigned char* enddata = NULL;
	int rc = 0;
	int mylen;

	FUNC_ENTRY;
	header.byte = readChar(&curdata);
	*dup = header.bits.dup;
	*packettype = header.bits.type;

	curdata += (rc = MQTTPacket_decodeBuf(curdata, &mylen)); /* read remaining length */
	enddata = curdata + mylen;

	if (enddata - curdata < 2)
		goto exit;
	*packetid = readInt(&curdata);

	rc = 1;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}


#include "ZPLENetClient.h"

#define ENET_IMPLEMENTATION
#include "../libs/enet.h"

#define TIMEOUT 5000

void ZPLENetClient::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("connect_to", "String", "int", "int", "int"), &ZPLENetClient::connect_to); // TODO: DEFAULT timeout ARG
	godot::ClassDB::bind_method(D_METHOD("send", "PackedByteArray", "int", "bool"), &ZPLENetClient::send); // TODO: DEFAULT reliable ARG
	// TODO
}

Error ZPLENetClient::connect_to(const String& ip, int port, int channels, int timeout = 5000) {
	// TODO
}

Error ZPLENetClient::send(const PackedByteArray& data, int channel, bool reliable = true) {
	// TODO
}

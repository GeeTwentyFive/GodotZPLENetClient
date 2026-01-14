#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "../libs/enet.h"

using namespace godot;

class ZPLENetClient : public RefCounted {
	GDCLASS(ZPLENetClient, RefCounted)

private:
	ENetHost* client = nullptr;
	ENetPeer* server_peer = nullptr;

	ENetEvent event;

public:
	ZPLENetClient() = default;

	Error connect_to(const String& ip, int port, int channels, int timeout = 5000);
	Error send(const PackedByteArray& data, int channel, bool reliable = true);
	Array service(); // First byte of each returned PackedByteArray is channel ID

	~ZPLENetClient();


protected:
	static void _bind_methods();
};

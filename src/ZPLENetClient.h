#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"

using namespace godot;

class ZPLENetClient : public RefCounted {
	GDCLASS(ZPLENetClient, RefCounted)

protected:
	static void _bind_methods();

public:
	ZPLENetClient() = default;
	~ZPLENetClient() override = default;

	Error connect_to(const String& ip, int port, int channels, int timeout = 5000);
	Error send(const PackedByteArray& data, int channel, bool reliable = true);
	// TODO: Separate thread or async for servicing + Connect, Disconnect, and Receive signals
};

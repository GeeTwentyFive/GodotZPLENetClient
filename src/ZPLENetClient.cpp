#define ENET_IMPLEMENTATION
#include "ZPLENetClient.h"


static int _enet_refcount = 0;


Error ZPLENetClient::connect_to(const String& ip, int port, int channels, int timeout) {
	if (_enet_refcount == 0) {
		if (enet_initialize() != 0) {
			return Error::FAILED;
		}
	}
	_enet_refcount++;

	client = enet_host_create(nullptr, 1, 1, 0, 0);
	if (!client) {
		_enet_refcount--;
		if (_enet_refcount == 0) enet_deinitialize();
		return Error::ERR_CANT_ACQUIRE_RESOURCE;
	}

	ENetAddress address = {0};
	enet_address_set_host(&address, ip.ascii().get_data());
	address.port = port;
	server_peer = enet_host_connect(client, &address, 1, 0);
	if (!server_peer) {
		enet_host_destroy(client);
		_enet_refcount--;
		if (_enet_refcount == 0) enet_deinitialize();
		return Error::ERR_CANT_ACQUIRE_RESOURCE;
	}

	if (!(
		enet_host_service(client, &event, timeout) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT
	)) {
		enet_peer_disconnect(server_peer, 0);
		enet_host_flush(client);
		enet_host_destroy(client);
		_enet_refcount--;
		if (_enet_refcount == 0) enet_deinitialize();
		return Error::ERR_CANT_CONNECT;
	}

	return Error::OK;
}

Error ZPLENetClient::send(const PackedByteArray& data, int channel, bool reliable) {
	if (!server_peer) return Error::ERR_UNAVAILABLE;

	ENetPacket* data_packet = enet_packet_create(
		data.ptr(),
		data.size(),
		reliable ? ENET_PACKET_FLAG_RELIABLE : 0
	);
	if (!data_packet) return Error::ERR_OUT_OF_MEMORY;
	enet_peer_send(server_peer, 0, data_packet);

	return Error::OK;
}

Array ZPLENetClient::service() {
	if (!client) return Array();

	Array received_data;

	while (enet_host_service(client, &event, 0) > 0) {
		if (
			event.type == ENET_EVENT_TYPE_DISCONNECT ||
			event.type == ENET_EVENT_TYPE_DISCONNECT_TIMEOUT
		) {
			server_peer = nullptr;
			enet_host_flush(client);
			enet_host_destroy(client);
			client = nullptr;
			emit_signal("disconnected");
			break;
		}

		if (event.type != ENET_EVENT_TYPE_RECEIVE) continue;

		PackedByteArray _received_data;
		_received_data.resize(1 + event.packet->dataLength);
		_received_data[0] = event.channelID;
		memcpy(
			1 + _received_data.ptrw(),
			event.packet->data,
			event.packet->dataLength
		);
		received_data.append(_received_data);

		enet_packet_destroy(event.packet);
	}

	return received_data;
}

ZPLENetClient::~ZPLENetClient() {
	if (server_peer) enet_peer_disconnect(server_peer, 0);

	if (client) {
		enet_host_flush(client);
		enet_host_destroy(client);
	}

	if (_enet_refcount > 0) {
		_enet_refcount--;
		if (_enet_refcount == 0) enet_deinitialize();
	}
}


void ZPLENetClient::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("connect_to", "ip", "port", "channels", "timeout"), &ZPLENetClient::connect_to, 5000); // TODO: DEFAULT timeout ARG
	godot::ClassDB::bind_method(D_METHOD("send", "data", "channel", "reliable"), &ZPLENetClient::send, true); // TODO: DEFAULT reliable ARG
	godot::ClassDB::bind_method(D_METHOD("service"), &ZPLENetClient::service);

	ADD_SIGNAL(MethodInfo("disconnected"));
}
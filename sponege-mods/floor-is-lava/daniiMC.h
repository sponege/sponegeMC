#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

namespace ffi {

enum class daniiMC_PacketType {
  HandshakeHandshake = 0,
  StatusRequest = 1,
  StatusResponse = 2,
  StatusPing = 3,
  StatusPong = 4,
  Unimplemented = -1,
};

enum class daniiMC_State {
  Handshake = 0,
  Status = 1,
  Unimplemented = -1,
};

using Packet = const void*;

using HandshakeHandshakePacket = const void*;

using StatusRequest = const void*;

using StatusResponse = const void*;

extern "C" {

int daniiMC_read(int file, Packet *packet_pointer, daniiMC_State state, bool to_server);

int daniiMC_write(int file, Packet packet);

daniiMC_PacketType daniiMC_getPacketKind(Packet packet);

HandshakeHandshakePacket daniiMC_getHandshakeHandshakePacket(Packet packet);

int daniiMC_getHandshakeHandshakePacketVersion(HandshakeHandshakePacket packet);

daniiMC_State daniiMC_getHandshakeHandshakePacketNextState(HandshakeHandshakePacket packet);

StatusRequest daniiMC_getStatusRequestPacket(Packet packet);

StatusResponse daniiMC_newStatusResponsePacket();

Packet daniiMC_packetStatusResponsePacket(StatusResponse packet);

void daniiMC_setStatusResponsePacketStatus(StatusResponse packet, const char *status);

} // extern "C"

} // namespace ffi

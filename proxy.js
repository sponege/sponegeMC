const net = require("net");
// const serverAddr = "garet.holiday";
// const serverPort = 25565;
const serverAddr = "localhost";
const serverPort = 1337;
// const serverAddr = "172.65.236.36"; // actual hypixel IP
// const serverPort = 25565;
// const serverAddr = "127.0.0.1"; // actual localhost ip
// const serverPort = 25565;
// const clientAddr = "mc.hypixel.net";
const clientAddr = "localhost";
const clientPort = 25563;

// function toArrayBuffer(buf) {
//   // https://stackoverflow.com/questions/8609289/convert-a-binary-nodejs-buffer-to-javascript-arraybuffer
//   const ab = new ArrayBuffer(buf.length);
//   const view = new Uint8Array(ab);
//   for (let i = 0; i < buf.length; ++i) {
//     view[i] = buf[i];
//   }
//   return ab;
// }

const SEGMENT_BITS = 0b01111111;
const CONTINUE_BIT = 0b10000000;

function readByte(buf) {
  if (buf.length == 0) return ["End of buffer reached!", buf];
  let byte = buf[0];
  buf = buf.slice(1); // slice one byte off the buffer
  return [byte, buf];
}

function readString(buf) {
  [length, buf] = readVarInt(buf);
  console.log("Length!", length);
  let string = buf.toString();
  console.log(string.length, length);
  console.log(string);
  if (string.length < length) return ["Buffer not yet complete!", ":("];
  return [string.substring(0, length), buf.slice(length)];
}

function readUnsignedShort(buf) {
  return [(buf[0] << 8) + buf[1], buf.slice(2)];
}

function readVarInt(buf) {
  let value = 0;
  let position = 0;
  let currentByte;

  while (true) {
    [currentByte, buf] = readByte(buf);

    if (typeof currentByte == "string") console.log(currentByte); // End of buffer reached!

    value |= (currentByte & SEGMENT_BITS) << position;

    if ((currentByte & CONTINUE_BIT) == 0) break;

    position += 7;

    if (position >= 32) throw "VarInt is too big";
  }

  return [value, buf];
}

var compression_on = false;
var threshold = null;

function processClientBoundPacket(buf) {
  var bufCopy = Buffer.alloc(Buffer.byteLength(buf));

  buf.copy(bufCopy);
  console.log(bufCopy);
  [length, bufCopy] = readVarInt(bufCopy);
  [packet_id, bufCopy] = readVarInt(bufCopy);
  console.log(length, packet_id);
  switch (packet_id) {
    case 0x0: // status
      [json, bufCopy] = readString(bufCopy);
      if (typeof bufCopy == "string") return [false, buf];
      console.log(`status! ${json}`);
      return [true, bufCopy];
    case 0x03: // enable compression
      [threshold, bufCopy] = readVarInt(bufCopy);
      compression_on = true;
      return [true, bufCopy];
    case 0x38: // player position

    default:
      // console.log("packet id not implemented yet");
      // return [true, readVarInt(buf)[1].slice(length)];
      return [false, buf];
  }
}

function processServerBoundPacket(buf) {
  var bufCopy = Buffer.alloc(Buffer.byteLength(buf));

  buf.copy(bufCopy);
  console.log(bufCopy);
  [length, bufCopy] = readVarInt(bufCopy);
  [packet_id, bufCopy] = readVarInt(bufCopy);
  console.log(length, packet_id);
  switch (packet_id) {
    case 0x0: // handshake
      [protocol_version, bufCopy] = readVarInt(bufCopy);
      [server_addr, bufCopy] = readString(bufCopy);
      [server_port, bufCopy] = readUnsignedShort(bufCopy);
      console.log(
        `handshake! ${protocol_version} ${server_addr} ${server_port}`
      );
      return [true, bufCopy];
    case 0x1: // status
      console.log(`server should send status now!`);
      return [true, bufCopy];
    default:
      console.log("no");
      return [false, buf];
  }
}

function processServerBoundBuffer(buf) {
  while (true) {
    if (buf.length == 0) {
      return buf;
    }
    [packetRead, buf] = processServerBoundPacket(buf);
    if (!packetRead) {
      return buf;
    }
  }
}

function processClientBoundBuffer(buf) {
  while (true) {
    if (buf.length == 0) {
      return buf;
    }
    [packetRead, buf] = processClientBoundPacket(buf);
    if (!packetRead) {
      return buf;
    }
  }
}

const server = net.createServer((client) => {
  var toProcessServerBound = Buffer.alloc(0);
  var toProcessClientBound = Buffer.alloc(0);

  const socket = net.createConnection(serverPort, serverAddr);

  console.log("-- Client connected.");

  socket.on("data", (data) => {
    console.log("<-", data.toString("hex").match(/../g).join(" "));
    toProcessClientBound = Buffer.concat([toProcessClientBound, data]);
    toProcessClientBound = processClientBoundBuffer(toProcessClientBound);
    client.write(data);
  });

  client.on("data", (data) => {
    console.log("->", data.toString("hex").match(/../g).join(" "));
    // sounds like a skill issue tbh
    toProcessServerBound = Buffer.concat([toProcessServerBound, data]);
    toProcessServerBound = processServerBoundBuffer(toProcessServerBound);
    // let packet = data.toString();
    // packet = packet.replace(clientAddr, serverAddr);
    // packet = Buffer.from(packet, "utf-8");
    // console.log("-?", packet);

    socket.write(data);
  });

  client.on("close", () => {
    console.log("-- Client disconnected.");
    socket.destroy();
  });

  socket.on("close", () => {
    console.log("-- Disconnected from server.");
    client.end();
  });

  socket.on("error", console.log);

  client.on("error", console.log);
});

server.listen(clientPort, clientAddr);

server.on("error", console.log);

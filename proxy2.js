#!/usr/bin/node

//import {createConnection, createServer} from "net";
const { createConnection, createServer } = require("net");
//import {argv, env, exit} from "process";
const { argv, env, exit } = require("process");
//import Hexy from "hexy";
const Hexy = require("hexy");
const FS = require("fs");

function never() {
  return new Promise(() => {});
}

async function main(environment, ...args) {
  if (args.length != 4) {
    console.log(`${argv[1]}: usage: ${argv[1]} <proxy from> <proxy to>`);
    return 1;
  }

  const [, , fromAddress, toAddress] = args;
  const [fromHost, fromPortRaw] = fromAddress.split(":");
  const fromPort = parseInt(fromPortRaw);
  const [toHost, toPortRaw] = toAddress.split(":");
  const toPort = parseInt(toPortRaw);

  if (isNaN(fromPort)) {
    console.log(`${argv[1]}: invalid from address: ${fromAddress}`);
    return 1;
  } else if (isNaN(toPort)) {
    console.log(`${argv[1]}: invalid to address: ${toAddress}`);
    return 1;
  }

  /*
	const server = createServer(fromSocket => {
		const toSocket = createConnection(toPort, toHost);
		console.log(`!! Connection from ${fromSocket.remoteAddress}:${fromSocket.remotePort} to ${toSocket.remoteAddress}:${toSocket.remotePort} established.`);

		toSocket.on("data", data => {
			const human = Hexy.hexy(data, {prefix: `<< `, format: "twos"});
			console.log(human.slice(0, human.length - 1));
			fromSocket.write(data);
		});

		fromSocket.on("data", data => {
			const human = Hexy.hexy(data, {prefix: `>> `, format: "twos"});
			console.log(human.slice(0, human.length - 1));
			toSocket.write(data);
		});

		toSocket.on("close", () => {
			console.log(`!! Connection closed.`);
			fromSocket.removeAllListeners();
			fromSocket.end();
		});

		fromSocket.on("close", () => {
			console.log(`!! Connection closed.`);
			toSocket.removeAllListeners();
			toSocket.end();
		});
	});*/
  const server = createServer((fromSocket) => {
    const toSocket = createConnection(toPort, toHost);
    console.log(
      `!! Connection from ${fromSocket.remoteAddress}:${fromSocket.remotePort} to ${fromHost}:${fromPort} established.`
    );
    let outgoing = { ref: Buffer.alloc(0), packets: 0 };
    let incoming = { ref: Buffer.alloc(0), packets: 0 };

    function process(buffer, direction) {
      while (true) {
        let length = 0n;
        let offset = 0;
        do {
          if (buffer.ref[offset] == undefined) return;
          length =
            (BigInt(buffer.ref[offset] & 0b01111111) << BigInt(7 * offset)) |
            length;
          if (offset > 10) return;
        } while ((buffer.ref[offset++] & 0b10000000) != 0);

        length = Number(length);
        if (buffer.ref.length >= length + offset) {
          const data = buffer.ref.slice(offset, offset + length);
          const human = Hexy.hexy(data, { prefix: direction, format: "twos" });
          console.log(human);
          buffer.ref = buffer.ref.slice(offset + length);
          console.log(`!! End of packet ${buffer.packets}.`);

          /*const packet = buffer.packets++;
					FS.open(`./${direction.trim()}/packet-${packet}.bin`, "w", 0o666, (err, fd) => {
						if (err) throw err;
						FS.writeFile(fd, data, (err) => {if (err) throw err;});
					});*/
        } else return;
      }
    }

    fromSocket.on("data", (data) => {
      outgoing.ref = Buffer.concat([outgoing.ref, data]);
      process(outgoing, ">> ");
      toSocket.write(data);
    });

    toSocket.on("data", (data) => {
      incoming.ref = Buffer.concat([incoming.ref, data]);
      process(incoming, "<< ");
      fromSocket.write(data);
    });

    toSocket.on("close", () => {
      console.log(`!! Connection closed.`);
      fromSocket.removeAllListeners();
      fromSocket.end();
    });

    fromSocket.on("close", () => {
      console.log(`!! Connection closed.`);
      toSocket.removeAllListeners();
      toSocket.end();
    });
  });
  server.listen(fromPort, fromHost);

  await never();
  return -1;
}

main(env, ...argv).then(exit, (error) => {
  console.error(error);
  exit(-1);
});

const fs = require('fs');
const SerialPort = require('serialport');
const yargs = require('yargs/yargs');
const { hideBin } = require('yargs/helpers');

const { argv } = yargs(hideBin(process.argv))

const portPath = argv.port || 'COM3';

const port = new SerialPort(portPath, { baudRate: 9600 });

const dumpBuffer = [];

const out = argv.out || './dump.bin';

const readCommandFromIndex = (index) => {
    return `R${index.toString(16)}%`
}

const read = index => {
    port.write(readCommandFromIndex(index));
}

let limit = argv.limit ? parseInt(argv.limit, 16) : 0x1F_FFFF;

port.on('data', data => {
    const number = data.toString('ASCII');

    const bin = Buffer.alloc(2);
    bin.writeUInt16LE(parseInt(number, 16));
    dumpBuffer.push(bin);

    if (index % 0x00_FFFF === 0) {
        console.log('Dumping...', `${Math.round(index / limit * 100)}%`);
    }

    if (index === limit) {
        const completeBuffer = Buffer.concat(dumpBuffer);

        fs.writeFileSync(out, completeBuffer);
        process.exit(0);
    } else {
        index += 1;
        read(index);
    }
})

// Set bank
if (argv.bank === 'HIGH') {
    port.write('BHI%');
} else {
    port.write('BLOW%');
}

let index = 0x00_0000

// Read first word
read(index);
const fs = require('fs');
const SerialPort = require('serialport');
const yargs = require('yargs/yargs');
const { hideBin } = require('yargs/helpers');

const { argv } = yargs(hideBin(process.argv))

const portPath = argv.port || 'COM3';

const port = new SerialPort(portPath, { baudRate: 9600 });

const dumpBuffer = [];

const readCommandFromIndex = (index) => {
    return `R${index.toString(16)}%`
}

const read = index => {
    port.write(readCommandFromIndex(index));
}

port.on('data', data => {
    const number = data.toString('ASCII');

    const bin = Buffer.alloc(2);
    bin.writeUInt16LE(parseInt(number, 16));

    console.log(number);

    process.exit(0);
})

// Set bank to low
port.write('BHI%');

let index = 0x00_000A

// Read first word
read(index);
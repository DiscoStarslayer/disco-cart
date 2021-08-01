const fs = require('fs');
const SerialPort = require('serialport');
const yargs = require('yargs/yargs');
const { hideBin } = require('yargs/helpers');

const { argv } = yargs(hideBin(process.argv))

const portPath = argv.port || 'COM3';

const port = new SerialPort(portPath, { baudRate: 4608000 });

const dumpBuffer = [];

const out = argv.out || './dump.bin';

const readCommandFromIndex = (index) => {
    return `R${index.toString(16)}%`
}

const read = index => {
    port.write(readCommandFromIndex(index));
}

const writeCommandFromIndex = (index, write) => {
    const str = `W${index.toString(16)}:${write.toString(16)}%`
    return str;
}

const write = (index, write) => {
    port.write(writeCommandFromIndex(index, write));
}

const erase = () => {
    port.write('ERS%');
}

let limit = argv.limit ? parseInt(argv.limit, 16) : 0x1F_FFFF;

const start = Date.now();
let timeElapsed = 0;

port.on('data', data => {
    if (argv.mode === 'dump') {
        const number = data.toString('ASCII');
        const bin = Buffer.alloc(2);
        bin.writeUInt16LE(parseInt(number, 16));
        dumpBuffer.push(bin);

        if (index % 0x00_FFF === 0) {
            timeElapsed = Date.now() - start;
            const percentCompleted = index / limit;

            const estTimeRemaining = Math.round((timeElapsed / percentCompleted) * (1 - percentCompleted));
            console.log('Dumping...', `${Math.round(index / limit * 100)}%,`, Math.round(estTimeRemaining / 1000 / 60), "minutes left");
        }

        if (index === limit) {
            const completeBuffer = Buffer.concat(dumpBuffer);

            fs.writeFileSync(out, completeBuffer);
            console.log('Done');
            process.exit(0);
        } else {
            index += 1;
            read(index);
        }
    } else if (argv.mode === 'write') {
        if (!erased) {
            console.log('Done');
            erased = true;
        }

        if (index % 0x00_FFFF === 0) {
            console.log('Writing...', `${Math.round((index / (rom.length / 2) * 100))}%`)
        }

        if (index === rom.length / 2) {
            console.log('Done');
            process.exit(0);
        } else {
            write(index, rom.readUInt16LE(index * 2));
            index++;
        }
    } else if (argv.mode === 'sddump') {
        const result = data.toString('ASCII');
        console.log(result);

        if (result.includes('Done!')) {
            process.exit(0);
        }
    }
})

// Set bank
if (argv.bank === 'HIGH') {
    port.write('BHI%');
} else {
    port.write('BLOW%');
}

let index = 0x00_0000
let erased = false;
let rom;

if (argv.mode === 'dump') {
    // Read first word
    read(index);
} else if (argv.mode === 'write') {
    rom = fs.readFileSync(argv.rom);

    console.log('Erasing...');
    erase();
} else if (argv.mode === 'sddump') {
    console.log(`Dumping file ${argv.name}:${argv.length}`);
    port.write(`D${argv.name}:${argv.length}`);
}
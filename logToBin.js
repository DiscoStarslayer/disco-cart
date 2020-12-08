const fs = require('fs');

const data = fs.readFileSync('./alphnew.log');

const lines = data.toString('ascii').split('\n').slice(3);
const binaryData = Buffer.alloc(lines.length * 2 * 16);

let offset = 0;
lines.forEach(line => {
    const columns = line.split('\t').slice(2, -1);

    columns.forEach(word => {
        binaryData.writeUInt16LE(parseInt(word, 16), offset);
        offset = offset + 2;
    })
});

fs.writeFileSync('./alphabet-park2.bin', binaryData);

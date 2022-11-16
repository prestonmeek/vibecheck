const fs = require('fs')

const PATH = 'E:\\edd\\edd-final-project\\audio-numbers'

fs.readdirSync(PATH).forEach(file => {
    if (file.endsWith('.mp3')) {
        /*
        parts = file.split('.')
        fs.renameSync(PATH + '\\' + file, PATH + '\\' + `${parts[0]}.${parts[2]}`)    // remove the .wav from 001.wav.mp3
        */

        parts = file.split('.')
        if (Number(parts[0]) >= 10) {
            fs.renameSync(PATH + '\\' + file, PATH + '\\' + file.slice(1))  // remove 0 from 0010 --> 010
        }
    }
    
})


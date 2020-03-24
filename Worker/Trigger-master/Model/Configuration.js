const mongoose = require('mongoose')

const SoilSchema = new mongoose.Schema({
    mac: { type: String },
    devices_name: { type: String },
    value_on: { type: String },
    value_off: { type: String }
})

module.exports = mongoose.model('config', SoilSchema)
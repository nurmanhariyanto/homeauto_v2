const mongoose = require('mongoose')

const HistorySchema = new mongoose.Schema({
    mac: { type: String },
    value: { type: String },
    waktu: {type : Date, default: new Date().toLocaleDateString()}
})

module.exports = mongoose.model('historys', HistorySchema)
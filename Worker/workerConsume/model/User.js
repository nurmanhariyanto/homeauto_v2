const mongoose = require('mongoose')

const UserSchema = new mongoose.Schema({

    guid: { type: String },
    no_ktp: { type: String },
    no_hp: { type: String },
    alamat: { type: String },
    nama: { type: String },
    email: { type: String },
    password: { type: String },
    latitude: { type: String },
    longitude: { type: String },
    user_slot: { type: String },
    devices: [
        {
            mac: { type: String },
            devices_name: { type: String },
            devices_code: { type: String },
            devices_type: { type: String },
            devices_registration_date: { type: Date },
            devices_activation_date: { type: Date, default: new Date().toLocaleDateString() }
            
        }]
})

module.exports = mongoose.model('users', UserSchema)
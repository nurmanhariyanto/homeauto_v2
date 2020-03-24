const amqp = require('amqplib//callback_api') //library untuk amq 
const mqtt = require('mqtt')
const express = require('express')
const body = require('body-parser')
const mongo = require('mongoose')
const User = require('./model/User')
const Jadwal = require('./model/Jadwal')
// var sensorController = require('./controller/simpancontroller')
const ObjectId = require('mongoose').Types.ObjectId
const app = express()
const fs = require('fs')
const uuid = require('uuid/v1')
var moment = require('moment')

app.use(body.urlencoded({ extended: false}))
app.use(body.json())

//config mqtt
var client = mqtt.connect('mqtt://167.205.7.226', {
    username: '/homeauto:homeauto',
    password: 'homeauto12345!',
    port: 1883
})

//config database
mongo.connect('mongodb://homeauto2:hom3auto2020!@dbmongo.server.pptik.id:27017/homeauto2', {
    useNewUrlParser: true,
    useUnifiedTopology: true
})

const db = mongo.connection
db.on('error', error => console.error(error)
)
db.once('open', () => console.log('Berhasil Terhubung ke database'))

//config amqp
amqp.connect({ protocol: 'amqp', hostname: '167.205.7.226', port: '5672', username: 'homeauto', password: 'homeauto12345!', vhost: '/homeauto' }, (err, conn) => {
    conn.createChannel((err, ch) => {
        const q = 'jadwal' //Nama queue untuk amq

        ch.assertQueue(q, { durable: true }) //menyatakan queue nya bernama task_queue
        ch.prefetch(1)
        console.log(`[*] Menunggu pesan di %s. untuk keluar tekan CTRL + C`, q)
        //Menangkap pesan yang dikirimkan rabbitmq dari antrian

        ch.consume(q, msg => {
            let message = msg.content.toString()
            var arr = message.split("#")
            var taskname = arr[0]
            var mac = arr[1]
            var zona = arr[2]
            var duration = arr[3]
            var repeatDaily = arr[4]
            var repeatDuration = arr[5]
            var repeatInterval = arr[6]
            var hourStart = arr[7]
            var minuteStart = arr[8]
            var trigger = arr[9]
            console.log(zona)
            
            var query = {"mac": mac, "zona.nomor_zona": zona }
            var value = { $set: { "zona.$.zona_name": taskname,"zona.$.Seconds_duration": duration, "zona.$.jadwal.id_jadwal": uuid(), "zona.$.jadwal.nama_jadwal": taskname , "zona.$.jadwal.hour_start": hourStart , "zona.$.jadwal.minute_start": minuteStart , "zona.$.jadwal.repeat_interval": repeatInterval , "zona.$.jadwal.repeat_duration": repeatDuration , "zona.$.jadwal.repeat_daily": repeatDaily , "zona.$.jadwal.trigger": trigger } };
            Jadwal.updateOne(query, value, function (err, res) {
                if (err) throw err
                console.log("1 Document inserted")
            })

            console.log(`[x] Menerima %s`, message)

            
            client.publish('setting_receiver', message)
                console.log(`[x] Done publish`)
        
            //Ack dan noAck untuk mengantisipasi saat koneksi mati queue masih tersimpan dan belum di hapus
            ch.ack(msg)
        })

    })
})
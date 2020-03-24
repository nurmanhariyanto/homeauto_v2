const mqtt = require('mqtt')
const mongo = require('mongoose')
const Configuration = require('./Model/Configuration')
const Waterlev = require('./Model/Waterlev')
const amqp = require('amqplib//callback_api')

mongo.connect('mongodb://homeauto2:hom3auto2020!@dbmongo.server.pptik.id:27017/homeauto2', {
    useNewUrlParser: true,
    useUnifiedTopology: true
})

var client = mqtt.connect('mqtt://shadoofpertanian.pptik.id', {
    username: '/shadoofpertanian:shadoofpertanian',
    password: 'TaniBertani19',
    port: 1883
})
const db = mongo.connection
db.on('error', error => console.error(error)
)
db.once('open', () => console.log('Berhasil Terhubung ke database'))

// var datanya = ""

amqp.connect({ protocol: 'amqp', hostname: 'shadoofpertanian.pptik.id', port: 5672, username: 'shadoofpertanian', password: 'TaniBertani19', vhost: '/shadoofpertanian' }, (err, conn) => {
    conn.createChannel((err, ch) => {
        const q = 'waterlev'
        const p = 'trigercallback'
        ch.assertExchange('waterlev')
        ch.assertQueue(q, { durable: true })
        ch.assertQueue(p, {durable: true})
        ch.prefetch(1)

        console.log("Menunggu Pesan", q)
        console.log("menunggu Pesan " , p)

        var datanya = ""
        //{"mac_act":"04FS_277f38_ssid3","mac_ss":"LSKK_AP2.1","value_on":"Lembab","value_off":"Lembab","guid":"e2aad3dd-da78-40b6-922d-2758b92eacf9","pesan":"true"}
        //consume callback kalau ada triger dari database
        ch.consume(p, msg => {
            const secs = msg.content.toString().split('.').length - 1
            //{"mac_act":"04FS_277f38_ssid3","mac_ss":"LSKK_AP2.1","value_on":"Lembab","value_off":"Lembab","guid":"e2aad3dd-da78-40b6-922d-2758b92eacf9","pesan":"true"}
            console.log(`[x] Menerima %s`, msg.content)
            var config = JSON.parse(msg.content.toString())
            var mac_act = config.mac_act
            var mac_ss = config.mac_ss
            var value_on = config.value_on
            var value_off = config.value_off
            var guid = config.guid
            let pesan = config.pesan
            
            if ( pesan == "false" || pesan == "False" || pesan == false ) {
                console.log("belum saatnya find")
            } 
            //logic kalau ada pesan true
            else if ( pesan == "true" || pesan == "True" || pesan == true) {
                var Value_off, Value_on
                //find data dari database
                Configuration.find({ mac: mac_ss}).then(data => {
                    try {
                        //nampung data dari database
                        datanya = data
                        for ( i in datanya){
                            Value_off = datanya[i].value_off
                            Value_on = datanya[i].value_on
                        }
                        // let datadb = JSON.parse(datanya)
                        //consume dari device
                        ch.consume(q, msg => {
                            const secs = msg.content.toString().split('.').length - 1
                            //
                            console.log(`[x] Menerima %s`, msg.content)
                            let tani = JSON.parse(msg.content.toString())
                            let mac = tani.MAC
                            let level = tani.Level_Air
                            let datalev = tani.data
                            let deviceId = tani.deviceId
                            var Waktu = new Date()
                            var perintah = ''
                            
                            //logic trigger nyamain data database dengan device
                                if (level == Value_on) {
                                    perintah = '0'
                                    console.log( "berhasil kirim " + perintah + " " + Waktu)
                                    client.publish(mac_act, perintah)
                                    
                                } else if (level == Value_off) {
                                    perintah = '1'
                                    console.log( "berhasil kirim " + perintah + " " + Waktu)
                                    client.publish(mac_act, perintah)  
                                } else {
                                    console.log("ga sesuai nih")
                                }
                
                            //schma
                            /* const water = new Waterlev({
                                mac: mac,
                                level_air: level,
                                data: datalev,
                                deviceId: deviceId
                            }) */
                
                            //masuk database
                            /* db.collection('history_waterlevs').insertOne(water, function (err, result) {
                                if (err) {
                                    console.log(err)
                                } else {
                                    console.log("1 Document inserted")
                                }
                            }) */
                
                            setTimeout(() => {
                                // ch.ack(msg)
                            }, secs * 1000)
                        })
                        
                    } catch (error) {
                        
                        console.log(error)
                    }
                }
                )
            }else{
                console.log("callback nya ga sesuai")
            }
            
            // console.log(datanya)

            setTimeout(() => {
                // ch.ack(msg)
            }, secs * 1000)
        })

        
    })
})


const mqtt = require('mqtt')
const mongo = require('mongoose')
const Waterlev = require('./Model/Waterlev')
const amqp = require('amqplib//callback_api')

mongo.connect('mongodb://homeauto2:hom3auto2020!@dbmongo.server.pptik.id:27017/homeauto2', {
    useNewUrlParser: true,
    useUnifiedTopology: true
})

const db = mongo.connection
db.on('error', error => console.error(error)
)
db.once('open', () => console.log('Berhasil Terhubung ke database'))

try {
    amqp.connect({ protocol: 'amqp', hostname: '167.205.7.226', port: 5672, username: 'homeauto', password: 'homeauto12345!', vhost: '/homeauto' }, (err, conn) => {
        conn.createChannel((err, ch) => {
            const q = 'feedback_device'
    
            // ch.assertExchange('waterlev')
            ch.assertQueue(q, { durable: true })
            ch.prefetch(1)
    
            console.log("Menunggu Pesan", q)
    
            ch.consume(q, msg => {
                const secs = msg.content.toString().split('.').length - 1
                //{"MAC":"84:f3:eb:e3:de:14","Level_Air":"","data":[0,0,0],"deviceId":["Sensor_WATER_001"]}
                console.log(`[x] Menerima %s`, msg.content)
                let message = msg.content.toString()
                var today = new Date()
                let msgg = JSON.parse(message);
                let mac = msgg.mac
                let value = msgg.value
                console.log(value)
    
                //schma
                const water = new Waterlev({
                    mac: mac,
                    value: value
                })
    
                //masuk database
                db.collection('historys').insertOne(water, function (err, result) {
                    if (err) {
                        console.log(err)
                    } else {
                        console.log("1 Document inserted")
                    }
                })
    
                setTimeout(() => {
                    ch.ack(msg)
                    //publish memakai mqtt
                }, secs * 1000)
            })
        })
    })    
} catch (error) {
    
}



sc.stop()
conf=SparkConf().setMaster("local[*]").setAppName("DelayRecProject")
sc=SparkContext(conf=conf)
sc.setLogLevel("WARN")


class DelayRec :
    year=""
    month=""
    dayOfMonth=""
    dayOfWeek=""
    crsDepTime=""
    depDelay=""
    origin=""
    distance=""
    cancelled=""

    holidays = List("01/01/2009", "01/21/2009", "02/12/2009", "05/30/2009",
     "06/07/2009", "07/04/2009","09/05/2009", "10/10/2009" ,"11/11/2009", "11/24/2009", "12/25/2009")

    def gen_features(): 
        values = Array(
            depDelay.toDouble,
            month.toDouble,
            dayOfMonth.toDouble,
            dayOfWeek.toDouble,
            get_hour(crsDepTime).toDouble,
            distance.toDouble,
            days_from_nearest_holiday(year.toInt, month.toInt, dayOfMonth.toInt)
        )
        Tuple2(to_date(year.toInt, month.toInt, dayOfMonth.toInt), values)

    def get_hour(depTime): 
        "%04d".format(depTime.toInt).take(2)
    def to_date(year, month, day) :
        "%04d%02d%02d".format(year, month, day)

    def days_from_nearest_holiday(year, month, day):
        sampleDate = datetime.datetime(year, month, day, 0, 0)

        holidays.foldLeft(3000) { (r, c) =>
            holiday = DateTimeFormat.forPattern("MM/dd/yyyy").parseDateTime(c)
            distance = Math.abs(Days.daysBetween(holiday, sampleDate).getDays)
            math.min(r, distance)
            }

    def prepFlightDelays(infile, sc):

        data = sc.textFile(infile)

        data.map { line =>
            reader = CSVReader(new StringReader(line))
            reader.readAll().asScala.toList.map(rec => DelayRec(rec(0),rec(1),rec(2),rec(3),rec(5),rec(15),rec(16),rec(18),rec(21)))
        }.map(list => list(0))
        .filter(rec => rec.year != "Year")
        .filter(rec => rec.cancelled == "0")
        .filter(rec => rec.origin == "ORD")
    }

    def parseData(vals: Array[Double]): LabeledPoint = {
        LabeledPoint(if (vals(0)>=15) 1.0 else 0.0, Vectors.dense(vals.drop(1)))
}
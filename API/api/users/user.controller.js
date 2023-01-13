const { getAllValues, newValue } = require("./user.service");

module.exports = {
  getAllValues: (req, res) => {
    getAllValues((err, results) => {
      if (err) {
        console.log(err);
        return;
      }
      console.log(results);
      return res.json({
        success: 1,
        data: results
      });
    });
  },
  newValue: (req, res) => {

    const measured_humidity = req.body.measured_humidity ?? 0.0;
    const measured_tank = req.body.measured_tank ?? 0.0;
    const measured_light = req.body.measured_light ?? 0.0;
    const current_temp = req.body.current_temp ?? 0.0;
    const current_rain = req.body.current_rain ?? 0.0;

    newValue(measured_value, (err, results) =>{
    if (err) {
        console.log(err);
        return res.status(500).json({
            success: 0,
            message: "Database connection error"
        });
    }
    else{
        return res.status(200).json({
            success: 1,
            message: "DB OK"
        });
    }
  })
  },
};

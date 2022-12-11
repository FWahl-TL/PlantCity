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
    const measured_value = req.body.measured_value;
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

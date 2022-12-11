const pool = require("../../config/database");

module.exports = {
  getAllValues:(callBack) => {
    pool.query(
      `SELECT * FROM data`,
      [],
      (error, results, fields) => {
        if (error) {
          callBack(error);
        }
        return callBack(null, results);
      }
    );
  },
  newValue:(measured_value, callBack) => {
    pool.query(
      `INSERT INTO data (measured_value) VALUES (?)`,
      [
        measured_value
      ],
      (error, results, fields) => {
        if (error) {
          callBack(error);
        }
        return callBack(null, results[0]);
      }
    );
  },
};


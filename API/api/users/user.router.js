const router = require("express").Router();
const { checkToken } = require("../../auth/token_validation");
const { getAllValues, newValue } = require("./user.controller");
router.get("/all", getAllValues);
router.post("/new", newValue);
module.exports = router;

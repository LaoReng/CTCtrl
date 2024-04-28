/*校验手机号码*/
export function checkMobile(phone) {
    var reg_tel =
        /^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\d{8}$/
    if (reg_tel.test(phone)) {
        return true;
    }
    else return false;
}
<template>
  <div class="personal-center">
    <el-form :model="userInfo" label-position="top" :rules="rules" ref="userInfo">
      <el-form-item class="ava" label="头像" style="height: 100%;">
        <div style="display: flex;align-items: center;justify-content: space-between;">
          <el-upload class="avatar-uploader" name='file' :file-list="fileList" action="http://81.70.91.154:80/api"
            :show-file-list="false" :on-success="handleAvatarSuccess" :before-upload="beforeAvatarUpload">
            <img v-if="userInfo.avatar" :src="userInfo.avatar
              " class="avatar">
            <i v-else class="el-icon-plus avatar-uploader-icon"></i>
          </el-upload>
          <div class="card">
            <div class="currentMoney">当前余额{{ userInfo.balance == 0 ? userInfo.balance.toFixed(1) : userInfo.balance }}元
            </div>
            <div class="registerTime">您已跨时空{{ currentDay ? currentDay : 0 }}天</div>
          </div>
        </div>
      </el-form-item>

      <el-form-item label=" 用户名" prop="name">
        <el-input v-direction="{ x: 0, y: 0 }" v-model="userInfo.name"></el-input>
      </el-form-item>

      <el-form-item label="性别" prop="sex">
        <el-select v-model="userInfo.sex" placeholder="请选择性别">
          <el-option label="不愿透露" value="2"></el-option>
          <el-option label="男" value="1"></el-option>
          <el-option label="女" value="0"></el-option>
        </el-select>
      </el-form-item>


      <el-form-item label="电话" prop="phone">
        <el-input v-direction="{ x: 0, y: 1 }" v-model="userInfo.phone"></el-input>
      </el-form-item>

      <el-form-item label="当前密码">
        <div style="display: flex;">
          <el-input style="min-width: 12rem;" inline v-model="userInfo.currentPassword" disabled></el-input>
          <el-button style="margin-left: 10rem;" size="mini" @click="showPassword = !showPassword">{{
            showPassword ? '返回' : '修改密码' }}</el-button>
        </div>
      </el-form-item>


      <!-- 是否显示密码 -->
      <div v-show="showPassword">
        <el-form-item label="密码" prop="password">
          <el-input show-password type="password" v-model="userInfo.password" autocomplete="off"></el-input>
        </el-form-item>
        <el-form-item label="确认密码" prop="repassword">
          <el-input show-password type="password" v-model="userInfo.repassword" autocomplete="off"></el-input>
        </el-form-item>
      </div>

      <el-form-item label="地址" prop="address">
        <el-cascader size="large" :options="pcaTextArr" v-model="userInfo.address">
        </el-cascader>
      </el-form-item>

      <el-form-item>
        <el-button type="primary" @click="submitForm">保存</el-button>
      </el-form-item>

    </el-form>
  </div>
</template>

<script>
import {
  provinceAndCityData,
  pcTextArr,
  regionData,
  pcaTextArr,
  codeToText,
} from "element-china-area-data";
import { get, post } from '@/plugins/request';
import emitter from '../../../plugins/mitt';

export default {
  data() {
    return {

      fileList: [{ name: 'food.jpeg', url: 'https://fuss10.elemecdn.com/3/63/4e7f3a15429bfda99bce42a18cdd1jpeg.jpeg?imageMogr2/thumbnail/360x360/format/webp/quality/100' }, { name: 'food2.jpeg', url: 'https://fuss10.elemecdn.com/3/63/4e7f3a15429bfda99bce42a18cdd1jpeg.jpeg?imageMogr2/thumbnail/360x360/format/webp/quality/100' }],
      currentDay: '',
      //显示隐藏修改密码
      showPassword: false,
      //地址存储的信息
      pcaTextArr,
      //表单验证规则
      rules: {
        name: [
          { required: true, validator: this.checkUsername, trigger: 'blur' }
        ],
        phone: [
          { required: true, validator: this.checkPhone, trigger: 'blur' }
        ],
        password: [
          {
            required: true,
            validator: this.validatePass,
            trigger: 'blur'
          }
        ],
        repassword: [
          {
            required: true,
            validator: this.validatePass2,
            trigger: 'blur'
          }
        ],
      },
      //表单信息
      userInfo: {
        avatar: "",
        name: "",
        sex: "",//1男，0女 2不愿透露
        phone: "",
        balance: 0,
        address: [],//这里传递的是一个数组
        password: "",//密码
        repassword: "",//重复密码
        currentPassword: "",//当前密码
        registerTime: "",
        status: "",
        message: ""
      },
      newObj: {},
      oldObj: {}
    };
  },
  methods: {
    //计算两个日期
    calculateDifferenceInDays(date1Str, date2Str) {
      // console.log(date1Str);
      var date1 = new Date(date1Str);
      var date2 = new Date(date2Str);
      // console.log(date1);
      // 计算两个日期之间的差值（以毫秒为单位）
      var differenceInMilliseconds = Math.abs(date2 - date1);

      // 将毫秒转换为天数（除以一天的毫秒数）
      var oneDayInMilliseconds = 24 * 60 * 60 * 1000; // 每天的毫秒数
      var differenceInDays = Math.floor(differenceInMilliseconds / oneDayInMilliseconds);
      return differenceInDays;
    },

    //处理图片成功
    handleAvatarSuccess(response, file, fileList) {
      // console.log(URL.createObjectURL(file.raw));
      console.log(response, URL.createObjectURL(file.raw));
      this.userInfo.avatar = URL.createObjectURL(file.raw);
    },
    //处理图片之前
    beforeAvatarUpload(file) {
      const isJPG = file.type === 'image/jpeg';
      const isLt2M = file.size / 1024 / 1024 < 2;

      if (!isJPG) {
        this.$message.error('上传头像图片只能是 JPG 格式!');
      }
      if (!isLt2M) {
        this.$message.error('上传头像图片大小不能超过 2MB!');
      }
      return isJPG && isLt2M;
    },
    //记录新内容的函数
    noteNewContent() {
      this.newObj.addr = this.userInfo.address;
      this.newObj.avatar = this.userInfo.avatar;
      this.newObj.balance = this.userInfo.balance;
      this.newObj.name = this.userInfo.name;
      if (this.userInfo.password == "") {
        this.newObj.password = this.userInfo.currentPassword
      }
      else {
        this.newObj.password = this.userInfo.password;
      }
      this.newObj.sex = this.userInfo.sex;
      this.newObj.registerTime = this.userInfo.registerTime;
      this.newObj.status = this.userInfo.status;
      this.newObj.message = this.userInfo.message;
      this.newObj.phone = this.userInfo.phone;
    },
    //计算diff
    diffUpdate(newObj, oldObj) {
      // console.log(newObj, oldObj);
      let obj = {};
      for (let k in newObj) {
        if (newObj[k].toString() != oldObj[k].toString()) {
          obj[k] = newObj[k];
        }
      }
      return obj;
    },
    //这里是后台传给我们的地址字符串，用逗号分隔
    addressShiftArray(str) {
      if (str.trim() == "") {
        return str.trim();
      }
      else {
        let res = str.split(",");
        return res;
      }
    },
    //这里是我们地址数组转变成逗号的字符串，传给后端
    ArrayShiftStr(arr) {
      return arr.toString();
    },
    //验证第一个密码
    validatePass(rule, value, callback) {
      //   console.log(this.$refs);
      //   console.log(value, callback());
      if (this.showPassword && value == "") {
        callback(new Error("请输入密码"));
      } else {
        // console.log(this.$refs.userInfo);
        if (this.userInfo.repassword !== "") {
          this.$refs.userInfo.validateField("repassword");
        }
        callback();
      }
    },
    //验证第二个密码
    validatePass2(rule, value, callback) {
      if (this.showPassword && (value === "")) {
        callback(new Error("请再次输入密码"));
      } else if (value != this.userInfo.password) {
        callback(new Error("两次输入密码不一致!"));
      } else {
        callback();
      }
    },
    //验证手机号
    checkPhone(rule, value, callback) {
      var reg_tel =
        /^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\d{8}$/; //11位手机号码正则
      if (value == "") {
        callback(new Error("请输入你的手机号"));
      } else if (!reg_tel.test(value)) {
        callback(new Error("请正确填写您的手机号码！"));
      }
      else {
        callback()
      }
    },
    //验证用户名
    checkUsername(rule, value, callback) {
      if (!value) {
        return callback(new Error('用户名不能为空'));
      }
      else {
        callback();
      }
    },
    //修改事件发起
    submitForm() {
      //如果我们什么也不进行修改
      this.noteNewContent();
      let diffObj = this.diffUpdate(this.newObj, this.oldObj);
      if (JSON.stringify(diffObj) == "{}") {
        this.$message({
          message: "当前信息没有修改！",
          type: "info",
          duration: 1000,
          showClose: true,
        })
        return;
      }
      this.$refs.userInfo.validate(async (res) => {
        //如果我没有点击修改密码，并且此时的内容信息不为空
        // console.log(res);
        if (!this.showPassword && res == true) {


          // this.noteNewContent();
          // let diffObj = this.diffUpdate(this.newObj, this.oldObj);
          // console.log(diffObj);//找出修改的对象
          //地址字符串化
          if (Array.isArray(diffObj.addr)) {
            diffObj.addr = this.ArrayShiftStr(diffObj.addr);
          }


          //发送请求
          let result = await post("/updateUserInfo", diffObj)
          if (result.status == 1) {
            this.$message({
              showClose: true,
              message: '修改成功',
              type: 'success'
            });
            this.init();
            this.showPassword = false;
            //如果传入了手机号，那么我们就等待当前请求过来再更新
            if (diffObj.phone) {
              localStorage.setItem('token', diffObj.phone)
            }
            //告知header组件更新数据
            emitter.emit('changeInfo')
          }
          else {
            this.$message({
              showClose: true,
              message: result.message,
              type: 'error'
            });
          }
        }
        //如果我没有点击修改密码，并且此时的内容信息为空
        else if (!this.showPassword && res == false) {
          this.$message({
            showClose: true,
            message: '内容信息不为空，请填写完整',
            type: 'error'
          });
        }
        //如果我点击了修改密码，并且此时的内容信息为空
        else if (this.showPassword && res == false) {
          this.$message({
            showClose: true,
            message: '内容信息不为空，请填写完整',
            type: 'error'
          });
        }
        //点击了修改密码，内容信息不为空，需要做判断，如果密码和之前的相同，就告诉他密码和原密码相同，否则发送请求
        else {
          //如果密码相同
          //这里获取接口数据，如果接口中的密码和我本地的密码相同，就告知密码相同
          if (this.userInfo.currentPassword == this.userInfo.password) {
            this.$message({
              message: "当前密码和原密码相同",
              type: "error"
            })
          }

          else {
            //发送请求
            this.noteNewContent();
            let diffObj = this.diffUpdate(this.newObj, this.oldObj);
            // console.log(diffObj);
            if (Array.isArray(diffObj.addr)) {
              diffObj.addr = this.ArrayShiftStr(diffObj.addr);
            }
            let res = await post("/updateUserInfo", diffObj)
            if (res.status == 1) {
              this.$message({
                showClose: true,
                message: '修改成功',
                type: 'success'
              });
              this.init();
              //清空修改密码之后的表单
              this.userInfo.password = "";
              this.userInfo.repassword = ""
              this.showPassword = false;
              //告知header组件更新数据
              emitter.emit('changeInfo')
            }
            else {
              this.$message({
                showClose: true,
                message: res.message,
                type: 'error'
              });
            }
          }
        }
      });
    },
    //初始化
    async init() {
      //得到接口数据
      let result = await get("/getUserInfo");
      console.log(result);
      if (result.status == 1) {
        this.userInfo.avatar = result.avatar;
        this.userInfo.balance = result.balance;
        this.userInfo.name = result.name;
        this.userInfo.currentPassword = result.password;
        this.userInfo.phone = result.phone;
        this.userInfo.registerTime = result.registerTime;
        this.userInfo.sex = result.sex;
        this.userInfo.avatar = result.avatar;
        this.userInfo.status = result.status;
        this.userInfo.message = result.message
        this.userInfo.address = this.addressShiftArray(result.addr)
        // console.log(this.userInfo.address);
        this.oldObj = { ...result };
        // console.log(this.oldObj);
        this.currentDay = this.calculateDifferenceInDays(this.userInfo.registerTime.replace(" ", 'T'),
          new Date().toLocaleString().replaceAll("/", "-"))
      }
      else {

      }

    }
  },
  //初始化事件
  mounted() {
    this.init();


  },
  created: function () {
    let direction = this.$getDirection()
    direction.on('keydown', function (e, val) {
      // console.log(val);
      // console.log(direction.next());
      //上
      if (e.keyCode === 38) {
        direction.previousLine()
      }
      //下
      if (e.keyCode === 40) {
        direction.nextLine()
      }
    })
  }
}
</script>

<style scoped>
.card {
  margin-left: 10rem;
  width: 50%;
  height: 12.7rem;
  min-width: 12.7rem;
}

.card div {
  letter-spacing: 3px;
  font-size: 1.3rem;
  font-family: STHupo;
  color: #ff9640;
}

.card .currentMoney {}

.card .registerTime {}

/* 图片 */

.avatar-uploader .el-upload {
  border: 1px dashed #d9d9d9;
  border-radius: 6px;
  cursor: pointer;
  position: relative;
  overflow: hidden;
}

.avatar-uploader .el-upload:hover {
  border-color: #409EFF;
}

.avatar-uploader-icon {
  font-size: 2rem;
  color: #8c939d;
  width: 12rem;
  height: 12rem;
  line-height: 12rem;
  border: 1px solid rgb(220, 209, 209);
  text-align: center;
}

.avatar {
  width: 12rem;
  height: 12rem;

  display: block;
}

/*  */
.personal-center {
  padding: 15px 30px 0;
}

.ava {
  height: 90px;
}
</style>

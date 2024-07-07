<template>
  <div>
    <el-dialog title="选择头像" v-model="isShowAvatarDialog" width="30%">
      <div class="imglist">
        <div class="list">
          <div
            class="img"
            style="
              display: grid;
              grid-template-columns: repeat(5, 5fr);
              gap: 2px;
            "
          >
            <div v-for="(item, index) in avatarList" :key="index">
              <!-- 让当前选中图片加个高亮 -->
              <div>
                <img
                  :class="{ highlighted: index === selectedAvatarIndex }"
                  @click="clickAvatar(item, index)"
                  style="width: 100%"
                  :src="`${worldAllData}` + item.url"
                />
              </div>
            </div>
          </div>
        </div>
      </div>
      <template v-slot:footer>
        <span class="dialog-footer">
          <el-button @click="isShowAvatarDialog = false">取 消</el-button>
          <el-button
            type="primary"
            :disabled="avatar.length == 0"
            @click="confirmDialog"
          >
            确定
          </el-button>
        </span>
      </template>
    </el-dialog>
    <div class="personal-center">
      <el-form
        :model="userInfo"
        label-position="top"
        :rules="rules"
        ref="userInfoForm"
      >
        <el-form-item class="ava" label="头像" style="height: 100%">
          <div
            style="
              display: flex;
              align-items: center;
              justify-content: space-between;
            "
          >
            <el-icon
              @click="isShowAvatarDialog = true"
              class="avatar-uploader-icon"
            >
              <img
                :src="
                  confirmAvatarSave == ''
                    ? worldAllData + userstore.userInfo.avatar
                    : worldAllData + confirmAvatarSave
                "
                class="avatar"
              />
              <p
                style="
                  position: absolute;
                  font-size: 40px;
                  color: rgb(245, 245, 245);
                  opacity: 0.6;
                "
              >
                +
              </p>
            </el-icon>

            <div class="card">
              <div class="currentMoney">
                当前余额{{
                  Number(userInfo.balance) == 0
                    ? Number(userInfo.balance).toFixed(1)
                    : userInfo.balance
                }}元
              </div>
              <div class="registerTime">
                您已跨时空{{ currentDay ? currentDay : 0 }}天
              </div>
            </div>
          </div>
        </el-form-item>

        <el-form-item label=" 用户名" prop="name">
          <el-input v-model="userInfo.name"></el-input>
        </el-form-item>

        <el-form-item label="性别" prop="sex">
          <el-select v-model="userInfo.sex" placeholder="请选择性别">
            <el-option label="不愿透露" value="2"></el-option>
            <el-option label="男" value="1"></el-option>
            <el-option label="女" value="0"></el-option>
          </el-select>
        </el-form-item>

        <el-form-item label="电话" prop="phone">
          <el-input v-model="userInfo.phone"></el-input>
        </el-form-item>

        <el-form-item label="当前密码">
          <div style="display: flex">
            <el-input
              style="min-width: 12rem"
              inline
              v-model="userInfo.currentPassword"
              disabled
            ></el-input>
            <el-button
              style="margin-left: 10rem"
              size="default"
              @click="showPassword = !showPassword"
            >
              {{ showPassword ? "返回" : "修改密码" }}
            </el-button>
          </div>
        </el-form-item>

        <!-- 是否显示密码 -->
        <div v-show="showPassword">
          <el-form-item label="密码" prop="password">
            <el-input
              show-password
              type="password"
              v-model="userInfo.password"
              autocomplete="off"
            ></el-input>
          </el-form-item>
          <el-form-item label="确认密码" prop="repassword">
            <el-input
              show-password
              type="password"
              v-model="userInfo.repassword"
              autocomplete="off"
            ></el-input>
          </el-form-item>
        </div>

        <el-form-item label="地址" prop="address">
          <el-cascader
            size="large"
            :options="pcaTextArr as CascaderOption[]"
            v-model="userInfo.address"
          ></el-cascader>
        </el-form-item>

        <el-form-item>
          <el-button type="primary" @click="submitForm">保存</el-button>
        </el-form-item>
      </el-form>
    </div>
  </div>
</template>

<script setup lang="ts">
import useUserStore from "@/store/modules/user/user";
import { updateUserInfoApi } from "@/api/user/index";
import { pcaTextArr } from "element-china-area-data";
import { CascaderOption, ElMessage } from "element-plus";
import type { personCenterRespType } from "@/api/user/type";
// import personCenterStore from "@/store/modules/personCenter/personCenter";
import { ref, reactive, onMounted } from "vue";
import { getUserInfoApi } from "@/api/user";
//用户仓库
const userstore = useUserStore();
//是否显示头像选择框
let isShowAvatarDialog = ref(false);
//点击确定临时暂存当前头像
let confirmAvatarSave: any = ref("");
//获取选取的头像
let avatar = ref("");
let avatarList: any = ref([]);
let selectedAvatarIndex = ref(-1);
let selectAvatarObj: any = ref({});
//显示隐藏修改密码
let showPassword = ref(false);

const confirmDialog = () => {
  isShowAvatarDialog.value = false;
  // userInfo.avatar = item.url;
  userInfo.avatar = selectAvatarObj.value.url;
  // console.log(userInfo.avatar);
  confirmAvatarSave.value = selectAvatarObj.value.url;
};
let newObj: personCenterRespType = reactive({});
let oldObj: any = reactive({});
//跨时空天数
let currentDay = ref("");
let userInfoForm = ref();
let userInfo = reactive({
  avatar: "",
  name: "",
  sex: "", //1男，0女 2不愿透露
  phone: "",
  balance: "",
  address: [], //这里传递的是一个数组
  password: "", //密码
  repassword: "", //重复密码
  currentPassword: "", //当前密码
  registerTime: "",
  status: 0,
  message: "",
});
//选取图片高亮
const clickAvatar = (item: any, index: any) => {
  selectedAvatarIndex.value = index == selectedAvatarIndex.value ? -1 : index;
  selectedAvatarIndex.value == -1
    ? (avatar.value = "")
    : (avatar.value = item.url);
  selectAvatarObj.value = { ...item };
  console.log(selectAvatarObj.value);
};
//验证用户名
const checkUsername = (_rule: any, value: any, callback: any) => {
  if (!value) {
    return callback(new Error("用户名不能为空"));
  } else {
    callback();
  }
};
//验证手机号
const checkPhone = (_rule: any, value: any, callback: any) => {
  const reg_tel =
    /^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\d{8}$/; //11位手机号码正则
  if (value == "") {
    callback(new Error("请输入你的手机号"));
  } else if (!reg_tel.test(value)) {
    callback(new Error("请正确填写您的手机号码！"));
  } else {
    callback();
  }
};
//验证第一个密码
const validatePass = (_rule: any, value: any, callback: any) => {
  if (showPassword.value && value == "") {
    callback(new Error("请输入密码"));
  } else {
    // console.log(this.$refs.userInfo);
    if (userInfo.repassword !== "") {
      userInfoForm.value.validateField("repassword");
    }
    callback();
  }
};
//验证第二个密码
const validatePass2 = (_rule: any, value: any, callback: any) => {
  if (showPassword.value && value === "") {
    callback(new Error("请再次输入密码"));
  } else if (value != userInfo.password) {
    callback(new Error("两次输入密码不一致!"));
  } else {
    callback();
  }
};
//表单校验规则
const rules = {
  name: [{ required: true, validator: checkUsername, trigger: "blur" }],
  phone: [{ required: true, validator: checkPhone, trigger: "blur" }],
  password: [
    {
      required: true,
      validator: validatePass,
      trigger: "blur",
    },
  ],
  repassword: [
    {
      required: true,
      validator: validatePass2,
      trigger: "blur",
    },
  ],
};
//记录新内容的函数
const noteNewContent = () => {
  newObj.addr = userInfo.address;
  newObj.avatar = userInfo.avatar;
  newObj.balance = Number(userInfo.balance);
  newObj.name = userInfo.name;
  if (userInfo.password == "") {
    newObj.password = userInfo.currentPassword;
  } else {
    newObj.password = userInfo.password;
  }
  newObj.sex = userInfo.sex;
  newObj.registerTime = userInfo.registerTime;
  newObj.status = userInfo.status.toString();
  newObj.message = userInfo.message;
  newObj.phone = userInfo.phone;
};
//计算diff
const diffUpdate = (newObj: any, oldObj: any): any => {
  // console.log(newObj, oldObj);
  let obj: any = {};
  for (let k in newObj) {
    if (newObj[k].toString() != oldObj[k].toString()) {
      obj[k] = newObj[k];
    }
  }
  return obj;
};
//这里是后台传给我们的地址字符串，用逗号分隔
const addressShiftArray = (str: string): string[] | string => {
  if (str.trim() == "") {
    return str.trim();
  } else {
    let res = str.split(",");
    return res;
  }
};
//这里是我们地址数组转变成逗号的字符串，传给后端
const ArrayShiftStr = (arr: object): string => {
  return arr.toString();
};
//计算两个日期
const calculateDifferenceInDays = (
  date1Str: string,
  date2Str: string,
): string => {
  // console.log(date1Str);
  let date1: Date = new Date(date1Str);
  let date2: Date = new Date(date2Str);
  // console.log(date1);
  // 计算两个日期之间的差值（以毫秒为单位）
  let differenceInMilliseconds = Math.abs(Number(date2) - Number(date1));

  // 将毫秒转换为天数（除以一天的毫秒数）
  let oneDayInMilliseconds = 24 * 60 * 60 * 1000; // 每天的毫秒数
  let differenceInDays = Math.floor(
    differenceInMilliseconds / oneDayInMilliseconds,
  );
  return differenceInDays.toString();
};
//初始化
const init = async () => {
  //得到接口数据
  let result = await getUserInfoApi();
  console.log(result);
  if (result.status == 1) {
    userInfo.avatar = result.avatar;
    userInfo.balance = result.balance;
    userInfo.name = result.name;
    userInfo.currentPassword = result.password;
    userInfo.phone = result.phone;
    userInfo.registerTime = result.registerTime;
    userInfo.sex = result.sex;
    userInfo.avatar = result.avatar;
    userInfo.status = result.status;
    userInfo.message = result.message;
    userInfo.address = addressShiftArray(result.addr) as [];
    // console.log(this.userInfo.address);
    oldObj = { ...result };
    // console.log(this.oldObj);
    currentDay.value = calculateDifferenceInDays(
      userInfo.registerTime.replace(" ", "T"),
      new Date().toLocaleString().replaceAll("/", "-"),
    ) as string;
  } else {
    console.log(1);
  }
};
//修改事件发起
const submitForm = async () => {
  //如果我们什么也不进行修改
  noteNewContent();
  let diffObj: any = diffUpdate(newObj, oldObj);
  if (JSON.stringify(diffObj) == "{}") {
    ElMessage({
      message: "当前信息没有修改！",
      type: "info",
      duration: 1000,
      showClose: true,
    });
    return;
  }
  //   console.log(res);
  // });
  // return;
  await userInfoForm.value.validate(async (res: any) => {
    //如果我没有点击修改密码，并且此时的内容信息不为空
    // console.log(res);
    if (!showPassword.value && res == true) {
      // this.noteNewContent();
      // let diffObj = this.diffUpdate(this.newObj, this.oldObj);
      // console.log(diffObj);//找出修改的对象
      //地址字符串化
      if (Array.isArray(diffObj.addr)) {
        diffObj.addr = ArrayShiftStr(diffObj.addr);
      }

      //发送请求
      let result = await updateUserInfoApi(diffObj);
      if (result.status == 1) {
        ElMessage({
          showClose: true,
          message: "修改成功",
          type: "success",
        });
        init();
        showPassword.value = false;
        //如果传入了手机号，那么我们就等待当前请求过来再更新
        if (diffObj.phone) {
          localStorage.setItem("token", diffObj.phone);
        }
        userstore.getUserInfo();
      } else {
        ElMessage({
          showClose: true,
          message: result.message,
          type: "error",
        });
      }
    }
    //如果我没有点击修改密码，并且此时的内容信息为空
    else if (!showPassword.value && res == false) {
      ElMessage({
        showClose: true,
        message: "内容信息不为空，请填写完整",
        type: "error",
      });
    }
    //如果我点击了修改密码，并且此时的内容信息为空
    else if (showPassword.value && res == false) {
      ElMessage({
        showClose: true,
        message: "内容信息不为空，请填写完整",
        type: "error",
      });
    }
    //点击了修改密码，内容信息不为空，需要做判断，如果密码和之前的相同，就告诉他密码和原密码相同，否则发送请求
    else {
      //如果密码相同
      //这里获取接口数据，如果接口中的密码和我本地的密码相同，就告知密码相同
      if (userInfo.currentPassword == userInfo.password) {
        ElMessage({
          message: "当前密码和原密码相同",
          type: "error",
        });
      } else {
        //发送请求
        noteNewContent();
        let diffObj = diffUpdate(newObj, oldObj);
        // console.log(diffObj);
        if (Array.isArray(diffObj.addr)) {
          diffObj.addr = ArrayShiftStr(diffObj.addr);
        }
        let res = await updateUserInfoApi(diffObj);
        if (res.status == 1) {
          ElMessage({
            showClose: true,
            message: "修改成功",
            type: "success",
          });
          init();
          //清空修改密码之后的表单
          userInfo.password = "";
          userInfo.repassword = "";
          showPassword.value = false;
          //更新
          userstore.getUserInfo();
        } else {
          ElMessage({
            showClose: true,
            message: res.message,
            type: "error",
          });
        }
      }
    }
  });
};
//生命周期

//全局遍历
let worldAllData = import.meta.env.VITE_APP_IMAGE;
onMounted(async () => {
  init();
  let res = await userstore.getUserAvatar();
  if (res.status == 0) {
    res.imgList.map((res) => {
      avatarList.value.push({
        id: res.id,
        url: res.url,
      });
    });
  }
  console.log(avatarList.value);
});
</script>

<style scoped>
.highlighted {
  border: 2px solid rgb(248, 77, 77); /* 示例：黄色边框作为高亮效果 */
  box-shadow: 0 0 5px rgba(241, 196, 15, 0.5); /* 示例：添加阴影增强高亮效果 */
}
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

.card .currentMoney {
}

.card .registerTime {
}

/* 图片 */

.avatar-uploader .el-upload {
  border: 1px dashed #d9d9d9;
  border-radius: 6px;
  cursor: pointer;
  position: relative;
  overflow: hidden;
}

.avatar-uploader .el-upload:hover {
  border-color: #409eff;
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

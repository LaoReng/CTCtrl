//路由鉴权
import router from "@/router";
//引入进度条
import nprogress from "nprogress";
//引入进度条样式
import "nprogress/nprogress.css";
//引入用户相关的仓库
import useAuthStore from "./store/auth";
import SettingStore from "./store/modules/setting";
//引入大仓库
import pinia from "./store";
import { ElMessage } from "element-plus";
//去掉加载转圈的小球
nprogress.configure({ showSpinner: false });
const userStore = useAuthStore(pinia);
const settingStore = SettingStore();
// console.log(userStore);
//获取用户电话号码
// const phone = userStore.userForm.phone;

//全局前置守卫
router.beforeEach(async (to: any, _from: any, next: any) => {
  //设置标题名字
  document.title = `CTCtrl后台--${to.meta.title}`;
  // console.log(_from);
  //访问某一个路由之前的触发
  nprogress.start();
  //! 获取token，判断用户登录还是未登录
  const token = userStore.token;
  //如果获取到了token，就让当前的登录状态变成true
  if (token) {
    settingStore.isLogin = true;
    // 如果登录了想去登录页面
    if (to.name == "login") {
      next({
        path: "/users",
      });
    }
    // 如果登录了去的不是登录页面
    else{
      next();
    }
   }
   // 如果没有登录,就跳转到登录页面
   else {
    // console.log(to);
    // next({
    //   path: "/login",
    // });
    if (to.name == "users" || to.name == "list" ||to.name == "apply" || to.name == "notices") {
      next({
        path: "/login",
      });
      ElMessage({
        type: "error",
        message: "请登录",
        duration: 1000,
        showClose: true,
      });
    } else {
      next();
    }
  }
});

//全局后置守卫
router.afterEach(() => {
  nprogress.done();
});
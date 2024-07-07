//路由鉴权
import router from "@/router";
//引入进度条
import nprogress from "nprogress";
//引入进度条样式
import "nprogress/nprogress.css";
//引入用户相关的仓库
import useUserStore from "./store/modules/user/user";
import SettingStore from "./store/modules/setting";
//引入大仓库
import pinia from "./store";
// import { ElMessage } from "element-plus";
// import { constants } from "http2";
//去掉加载转圈的小球
nprogress.configure({ showSpinner: false });
const userStore = useUserStore(pinia);
const settingStore = SettingStore();
// console.log(userStore);
//获取用户名字
// const username = userStore.userInfo.name;

const whiteList = ["/desc", "/download", "/newlogin", "/newregister", "/404"];

//全局前置守卫
router.beforeEach(async (to: any, _from: any, next: any) => {
  //设置标题名字
  // document.title = `CTCtrl-${to.meta.title}`;
  document.title = `CTCtrl`;
  const token = userStore.token;
  // console.log(token);
  //访问某一个路由之前的触发
  nprogress.start();
  //如果当前没有token
  if (token == null) {
    //如果访问的是在白名单范围内，就放行
    if (whiteList.includes(to.path)) {
      next();
    } else {
      //否则，就跳转到登录页面
      next({
        path: "/newlogin",
      });
    }
  }
  //有token
  else {
    // console.log(to, _from);
    if (to.path == "/newlogin" || to.path == "newregister") {
      next({
        path: "/home",
      });
    }
    // else {
    //获取信息失效
    try {
      await userStore.getUserInfo();
      settingStore.isLogin = true;
      next();
    } catch (error) {
      userStore.token = null;
      next({
        path: "/newlogin",
      });
    }
    // }
  }
  // ! 获取token，判断用户登录还是未登录
  // const token = userStore.token;
  //如果获取到了token，就让当前的登录状态变成true
  // if (token) {
  //   //!获取用户信息,保证头像不丢失和用户名
  //   await userStore.getUserInfo();
  //   settingStore.isLogin = true;
  //   next();
  // } else {
  //   if (to.name == "userInfo" || to.name == "vip" || to.name == "vnc") {
  //     next({
  //       path: "/home",
  //     });
  //     ElMessage({
  //       type: "error",
  //       message: "请登录",
  //       duration: 1000,
  //       showClose: true,
  //     });
  //   } else {
  //     next();
  // }
  // }
  // if (token) {
  //   //登录成功想访问login，不能让其访问，指向首页
  //   if (to.path == "/login") {
  //     next("/");
  //   } else {
  //     //如果有用户信息
  //     if (username) {
  //       next();
  //     }
  //     //获取不到用户信息,在守卫这里发起请求获取到了信息再放行
  //     else {
  //       try {
  //         //获取到用户信息之后，再放行
  //         await userStore.getUserInfo();
  //         next();
  //       } catch (error) {
  //         //token过期获取不到或者用户手动修改了本地存储
  //         //执行退出登录逻辑
  //         userStore.userLogout();
  //         next({
  //           path: "/login",
  //           query: {
  //             redirect: to.path,
  //           },
  //         });
  //       }
  //     }
  //   }
  // }
  // //未登录
  // else {
  //   if (to.path == "/login") {
  //     next();
  //   } else {
  //     next({
  //       path: "/login",
  //       query: {
  //         redirect: to.path,
  //       },
  //     });
  //   }
  // }
});
//全局后置守卫
router.afterEach(() => {
  nprogress.done();
});

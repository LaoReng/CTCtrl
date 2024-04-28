// import { from } from "core-js/core/array";
import Vue from "vue";
import VueRouter from "vue-router";
// import HomeView from "../views/HomeView.vue";
import Layout from "../layout/index.vue";
import NProgress from "nprogress";
NProgress.configure({ showSpinner: false });
Vue.use(VueRouter);

const routes = [
  {
    path: "/login",
    name: "login",
    component: () => import("../views/Login.vue"),
  },
  {
    path: "/register",
    name: "register",
    component: () => import("../views/Register.vue"),
  },
  {
    path: "/",
    name: "home",
    redirect: "/login",
    component: Layout,
    children: [
      {
        path: "user",
        name: "user",
        component: () => import("../views/system/users/PersonalCenter.vue"),
      },
      {
        path: "role",
        name: "role",
        component: () => import("../views/system/users/DownloadCenter.vue"),
      },
      {
        path: "dashboard",
        name: "dashboard",
        component: () => import("../views/system/equipment/Dashboard.vue"),
      },

      {
        path: "vip",
        name: "vip",
        component: () => import("../views/system/users/VipCenter.vue"),
      },
      {
        path: "notice",
        name: "notice",
        component: () => import('../views/system/notice/NoticeCenter.vue')
      },

    ],
  },
  {
    path: "/vnc",
    name: "vnc",
    component: () => import("../views/VncShow.vue"),
  }

];

const router = new VueRouter({
  routes,
});

//! 由于后台管理系统，需要用户登陆后才可以访问页面
//! 因此在这里要拦截所有的路由

router.beforeEach((to, from, next) => {

  let whiteList = ['/login', '/register']
  let token = localStorage.getItem('token');
  //如果有token
  if (token) {
    //如果有token并且要跳转的路径是登录
    if (whiteList.includes(to.path))
      next({ path: '/dashboard' });
    else {
      next();
    }
  }
  //没有token
  else {
    if (whiteList.includes(to.path)) {
      next();
    }
    else {
      next({
        path: "/login"
      })
    }
  }
});

export default router;

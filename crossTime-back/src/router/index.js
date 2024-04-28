// import { from } from "core-js/core/array";
import Vue from "vue";
import VueRouter from "vue-router";
// import HomeView from "../views/HomeView.vue";
import Layout from "../layout/index.vue";
import NProgress from "nprogress";
import Register from "..//views/Register.vue";
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
    redirect: "/user",
    component: Layout,
    children: [
      {
        path: "user",
        name: "user",
        component: () => import("../views/system/users/UserManagement.vue"),
      },
      {
        path: "device",
        name: "device",
        component: () => import("../views/system/devices/DeviceManagement.vue"),
      },
      {
        path: "notificat",
        name: "notificat",
        component: () =>
          import("../views/system/notificats/NotificatAnnouncement.vue"),
      },
    ],
  },
];

const router = new VueRouter({
  routes,
});

//! 由于后台管理系统，需要用户登陆后才可以访问页面
//! 因此在这里要拦截所有的路由

router.beforeEach((to, from, next) => {
  // console.log(to,from);
  if (to.path != from.path) {
    // router.push(to.path)
    next();
  }
  // else{
  //   next();
  // }
});

export default router;

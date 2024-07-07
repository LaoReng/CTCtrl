export const routes = [
  {
    path: "/login",
    name: "login", //用作路由拦截
    component: () => import("@/view/login/index.vue"),
    meta: {
      title: "登录", //菜单需要的标题
      hidden: true, //代表路由的标题在菜单中是否因此
      icon: "Promotion",
    },
  },
  {
    //登录成功展示数据的路由
    path: "/",
    name: "Layout",
    component: () => import("@/layout/index.vue"),
    meta: {
      title: "", //菜单需要的标题
      hidden: false,
      icon: "",
    },
    redirect: "/users",
    children: [
      {
        path: "/users",
        name: "users",
        component: () => import("@/view/users/index.vue"),
        meta: {
          title: "用户管理", //菜单需要的标题
          hidden: false,
          icon: "User",
        },
      },
    ],
  },
  {
    path: "/devices",
    name: "devices",
    meta: {
      title: "设备管理",
      hidden: false,
      icon: "Monitor",
    },
    redirect: "/devices/list",
    component: () => import("@/layout/index.vue"),
    children: [
      {
        path: "/devices/list",
        name: "list",
        component: () => import("@/view/devices/list/index.vue"),
        meta: {
          title: "设备列表", //菜单需要的标题
          hidden: false,
          icon: "Tickets",
        },
      },
      {
        path: "/devices/apply",
        name: "apply",
        component: () => import("@/view/devices/apply/index.vue"),
        meta: {
          title: "设备申请", //菜单需要的标题
          hidden: false,
          icon: "Monitor",
        },
      },
    ],
  },
  {
    path: "/notices",
    name: "notices",
    meta: {
      title: "通知管理",
      hidden: false,
      icon: "ChatLineSquare",
    },
    component: () => import("@/layout/index.vue"),
    redirect: "/notices/list",
    children: [
      {
        path: "/notices/list",
        name: "notices",
        component: () => import("@/view/notices/list/index.vue"),
        meta: {
          title: "通知管理", //菜单需要的标题
          hidden: false,
          icon: "Tickets",
        },
      },
    ],
  },
  {
    path: "/404",
    name: "404",
    component: () => import("@/view/404/index.vue"),
    meta: {
      title: "404", //菜单需要的标题
      hidden: true,
      icon: "MoreFilled",
    },
  },
  {
    path: "/:pathMatch(.*)*",
    redirect: "/404",
    name: "Any",
    meta: {
      title: "任意", //菜单需要的标题
      hidden: true,
      icon: "RemoveFilled",
    },
  },
];

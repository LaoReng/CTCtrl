import Login from "@/view/newlogin/index.vue";
export const routes = [
  {
    path: "/vnc",
    name: "vnc",
    component: () => import("@/view/vnc/index.vue"),
    meta: {
      title: "vnc", //菜单需要的标题
      hidden: true, //代表路由的标题在菜单中是否显示
      icon: "Promotion",
    },
  },
  {
    path: "/newlogin",
    name: "newlogin",
    // component: () => import("@/view/newlogin/index.vue"),
    component: Login,
    meta: {
      title: "新登录", //菜单需要的标题
      hidden: true, //代表路由的标题在菜单中是否显示
      icon: "Promotion",
    },
  },
  {
    path: "/newregister",
    name: "newregister", //用作路由拦截
    component: () => import("@/view/newregister/index.vue"),
    meta: {
      title: "注册", //菜单需要的标题
      hidden: true, //代表路由的标题在菜单中是否显示
      icon: "Promotion",
    },
  },
  {
    path: "/download",
    name: "download", //用作路由拦截
    component: () => import("@/view/download/index.vue"),
    meta: {
      title: "下载中心", //菜单需要的标题
      hidden: true, //代表路由的标题在菜单中是否显示
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
      icon: "Promotion",
    },
    redirect: "/home",
    children: [
      {
        path: "/home",
        name: "home",
        component: () => import("@/view/home/index.vue"),
        meta: {
          title: "控制台", //菜单需要的标题
          hidden: false,
          icon: "HomeFilled",
        },
      },
    ],
  },
  {
    path: "/user",
    name: "user",
    meta: {
      title: "个人中心",
      hidden: false,
      icon: "User",
    },
    redirect: "/user/userInfo",
    component: () => import("@/layout/index.vue"),
    children: [
      {
        path: "/user/userInfo",
        name: "userInfo",
        component: () => import("@/view/info/personCenter/index.vue"),
        meta: {
          title: "个人信息", //菜单需要的标题
          hidden: false,
          icon: "Avatar",
        },
      },
      {
        path: "/user/vip",
        name: "vip",
        component: () => import("@/view/info/vip/index.vue"),
        meta: {
          title: "VIP 中心", //菜单需要的标题
          hidden: false,
          icon: "Medal",
        },
      },
      {
        path: "/user/equip",
        name: "equip",
        component: () => import("@/view/info/equip/index.vue"),
        meta: {
          title: "设备管理", //菜单需要的标题
          hidden: false,
          icon: "Monitor",
        },
      },
    ],
  },
  {
    path: "/notice",
    name: "notice",
    component: () => import("@/layout/index.vue"),
    meta: {
      title: "", //菜单需要的标题
      hidden: false,
      icon: "Notification",
    },
    redirect: "/",
    children: [
      {
        path: "/mynotice",
        name: "mynotice",
        component: () => import("@/view/info/notice/index.vue"),
        meta: {
          title: "通知中心", //菜单需要的标题
          hidden: false,
          icon: "Notification",
        },
      },
    ],
  },
  {
    path: "/screen",
    name: "screen",
    component: () => import("@/view/newscreen/index.vue"),
    meta: {
      title: "数据大屏",
      hidden: false,
      icon: "Film",
    },
  },
  {
    path: "/embed",
    name: "embed",
    component: () => import("@/view/flushbonading/index.vue"),
    meta: {
      title: "嵌入式",
      hidden: true,
      icon: "Film",
    },
    redirect: "embed/home",
    children: [
      {
        path: "/embed/home",
        name: "/embed/home",
        // 冰箱主页面显示
        component: () => import("@/view/flushbonading/view/home/index.vue"),
        meta: {
          title: "home",
          hidden: false,
          icon: "Film",
        },
      },
      {
        path: "/embed/about",
        name: "about",
        // 冰箱主页面显示
        component: () => import("@/view/flushbonading/view/about/about.vue"),
        meta: {
          title: "about",
          hidden: false,
          icon: "Film",
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
    path: "/desc",
    name: "desc",
    component: () => import("@/view/description/index.vue"),
    meta: {
      title: "描述",
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

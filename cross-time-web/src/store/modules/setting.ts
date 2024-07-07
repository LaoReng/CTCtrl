import { defineStore } from "pinia";

const SettingStore = defineStore("Setting", {
  state: () => {
    return {
      fold: false,
      refresh: false, //刷新按钮
      isLogin: false, //是否登录
      toRegister: false, //是否点击了注册页面
      distanceX: 0, //距离左侧距离
      distanceY: 0, //距离右侧距离
    };
  },
  actions: {},
});
export default SettingStore;

import { defineStore } from "pinia";

const SettingStore = defineStore("Setting", {
  state: () => {
    return {
      fold: false,
      refresh: false, //刷新按钮
      isLogin: false, //是否登录
    };
  },
  actions: {},
});
export default SettingStore;

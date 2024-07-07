import { defineStore } from "pinia";
import { getAllNoticeApi, getNewNoticeApi } from "@/api/notice/index";
const noticeStore = defineStore("notice", {
  state: () => {
    return {};
  },
  actions: {
    //获取所有的通知
    async getAllNotice() {
      const res = await getAllNoticeApi();
      console.log(res);
      return Promise.resolve(res);
    },
    //获取最新通知
    async getNewNotice() {
      const res = await getNewNoticeApi();
      return Promise.resolve(res);
    },
  },
});
export default noticeStore;

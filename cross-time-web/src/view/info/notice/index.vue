<template>
  <div class="noticeContainer">
    <div v-if="isNotify" class="container">
      <el-table :data="notifications">
        <el-table-column
          prop="title"
          label="标题"
          width="290"
        ></el-table-column>
        <el-table-column
          prop="message"
          label="内容"
          width="auto"
          style="overflow: hidden; text-overflow: ellipsis; white-space: nowrap"
        ></el-table-column>
        <el-table-column
          prop="createdAt"
          label="创建时间"
          width="180"
        ></el-table-column>
      </el-table>
    </div>
    <div v-else>
      <el-empty
        description="暂无通知"
        style="
          position: absolute;
          left: 55%;
          top: 50%;
          transform: translate(-50%, -50%);
        "
      ></el-empty>
    </div>
  </div>
</template>

<script setup lang="ts">
import { onMounted, ref } from "vue";
import { AllNoticeArrType } from "@/api/notice/type";
import noticeStore from "@/store/modules/notice/notice";
let isNotify = ref(true);
let notice = noticeStore();
let notifications = ref<AllNoticeArrType[]>([]);
//初始化方法
const init = async () => {
  const res = await notice.getAllNotice();
  console.log(res);
  if (res.total == 0) {
    isNotify.value = false;
  } else {
    notifications.value = [...res.notice];
  }
};
onMounted(() => {
  init();
});
</script>

<style scoped></style>

<template>
  <div class="box">
    <div style="padding: 20px 20px 10px">
      <span style="font-size: 20px; margin-left: 10px">
        <i>通知列表</i>
      </span>
      <el-button
        class="right"
        plain
        @click="dialogFormVisibleF = true"
        type="primary"
        style="float: right; background: rgb(64, 158, 255); color: white"
        >发布通知</el-button
      >
      <el-button v-if="length" class="right" type="danger" @click="btnDel"
        >批量删除</el-button
      >
    </div>
    <!-- 列表 -->
    <div class="elm-box">
      <el-table
        :data="tableData"
        style="width: 100%; margin-top: -10px"
        @row-click="selectNotice"
        @selection-change="handleSelectionChange"
      >
        <el-table-column fixed="left" type="selection" width="55" />
        <el-table-column
          prop="noticeId"
          label="通知ID"
          width="100"
          show-overflow-tooltip
        />
        <el-table-column
          prop="admId"
          label="管理员ID"
          width="110"
          show-overflow-tooltip
        />
        <el-table-column
          prop="title"
          label="标题"
          width="180"
          show-overflow-tooltip
        />
        <el-table-column prop="content" label="内容">
          <template #default="scope">
            <el-tooltip
              effect="dark"
              :hide-after="10"
              :raw-content="true"
              :content="dealStr(scope.row.content)"
              placement="top"
            >
              <div class="content-cell">
                {{ scope.row.content }}
              </div>
            </el-tooltip>
          </template>
        </el-table-column>
        <el-table-column
          prop="createTime"
          label="创建时间"
          width="180"
          show-overflow-tooltip
        />
        <el-table-column
          prop="updateTime"
          label="更新时间"
          width="180"
          show-overflow-tooltip
        />
        <el-table-column fixed="right" label="&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;操作" width="135">
          <template #default>
            <el-button
              plain
              @click="dialogFormVisibleX = true"
              type="primary"
              size="small"
              >修改</el-button
            >
            <el-popconfirm
              width="220"
              confirm-button-text="确定"
              cancel-button-text="不了"
              title="你确定要删除此用户吗?"
              @confirm="handledelete()"
            >
              <template #reference>
                <el-button type="danger" size="small">删除</el-button>
              </template>
            </el-popconfirm>
          </template>
        </el-table-column>
      </el-table>
      <!-- 修改对话框 -->
      <el-dialog
        v-model="dialogFormVisibleX"
        title="通知修改"
        :label-width="formLabelWidth"
        width="500"
        draggable
      >
        <el-form :model="form2">
          <!-- <el-form-item label="通知ID" :label-width="formLabelWidth">
            <el-input v-model="selectedNoticeId" disabled></el-input>
          </el-form-item> -->
          <el-form-item
            label="标题"
            prop="title"
            :label-width="formLabelWidth"
            required
          >
            <el-input autocomplete="off" v-model="form2.title"></el-input>
          </el-form-item>
          <el-form-item
            label="内容"
            prop="content"
            :label-width="formLabelWidth"
            required
          >
            <el-input
              type="textarea"
              autocomplete="off"
              v-model="form2.content"
            ></el-input>
          </el-form-item>
          <el-form-item label="更新时间" :label-width="formLabelWidth">
            <el-input
              v-model="formattedTime"
              autocomplete="off"
              disabled
            ></el-input>
          </el-form-item>
        </el-form>
        <template #footer>
          <div class="dialog-footer">
            <el-button @click="dialogFormVisibleX = false">取消</el-button>
            <el-button
              type="primary"
              @click="(dialogFormVisibleX = false), updatenotice()"
            >
              修改
            </el-button>
          </div>
        </template>
      </el-dialog>
      <!-- 发布对话框 -->
      <el-dialog
        v-model="dialogFormVisibleF"
        title="通知发布"
        :label-width="formLabelWidth"
        width="500"
        draggable
      >
        <el-form :data="form1">
          <!-- <el-form-item label="手机号" :label-width="formLabelWidth">
            <el-input v-model="phone" disabled></el-input>
          </el-form-item> -->
          <el-form-item
            label="标题"
            prop="title"
            :label-width="formLabelWidth"
            required
          >
            <el-input autocomplete="off" v-model="form1.title"></el-input>
          </el-form-item>
          <el-form-item
            label="内容"
            prop="content"
            :label-width="formLabelWidth"
            required
          >
            <el-input
              type="textarea"
              autocomplete="off"
              v-model="form1.content"
            ></el-input>
          </el-form-item>
          <el-form-item label="发布时间" :label-width="formLabelWidth">
            <el-input
              v-model="formattedTime"
              autocomplete="off"
              disabled
            ></el-input>
          </el-form-item>
        </el-form>
        <template #footer>
          <div class="dialog-footer">
            <el-button @click="dialogFormVisibleF = false">取消</el-button>
            <el-button
              type="primary"
              @click="(dialogFormVisibleF = false), handleSubmit()"
            >
              发布
            </el-button>
          </div>
        </template>
      </el-dialog>
    </div>
    <!-- 分页 -->
    <div class="page-box">
      <el-pagination
        class="page"
        :page-size="pageSize"
        :current-page="currentPage"
        layout="prev, pager, next, jumper"
        :total="total"
        @current-change="handlePageChange"
      />
    </div>
  </div>
</template>

<script setup lang="ts">
import {
  noticeListApi,
  issueNoticeApi,
  updataNoticeApi,
  deleteNoticeApi,
} from "@/api/notice";
import pinia from "@/store";
import { useAuthStore } from "@/store/auth";
import { ElMessage } from "element-plus";
import { onBeforeMount, onMounted, ref } from "vue";
const userStore = useAuthStore(pinia);
const dialogFormVisibleX = ref(false);
const dialogFormVisibleF = ref(false);
const formLabelWidth = "68px";
let tableData = ref([
  {
    noticeId: "",
    admId: "",
    title: "",
    content: "",
    createTime: "",
    updateTime: "",
  },
]);
let total = ref(); // 总页数
let pageSize = ref(10); //每页显示多少条数据
let currentPage = ref(1); // 当前在第几页
// 发布
let form1 = ref({
  title: "",
  content: "",
  delivery: false,
});
let phone = ref(userStore.token);
// console.log(phone.value);
// 修改
let form2 = ref({
  title: "",
  content: "",
});
let selectedNoticeId = ref();
let selectedNoticeIds = ref<any[]>([]); //删除id数组
let formattedTime = ref(""); //实时时间
let length = ref(false); // 判断是否大于1个
// 初始化要发送到后端的数据对象
// 分页获取通知信息
async function getNoticeData(_page: number) {
  try {
    const response = await noticeListApi({
      page: currentPage.value,
      pageSize: pageSize.value,
    });
    // console.log("分页: ", response.data);
    if (response.status == 1) {
      tableData.value = response.data.list;
      total.value = response.data.total * 10;
      // console.log(total.value);
    } else {
      console.error("API返回数据格式不符预期");
    }
  } catch (error) {
    console.error("请求失败:", error);
  }
}
// 挂载之前获取数据
onBeforeMount(() => {
  getNoticeData(currentPage.value); // 首次加载第一页数据
});
// 当前页数变化就再加载一次
const handlePageChange = (newPage: number) => {
  currentPage.value = newPage;
  getNoticeData(newPage);
};

// 文字提示换行
function dealStr(content: string) {
  let result = "";
  let currentLineLength = 0;

  for (const char of content) {
    currentLineLength += 1;
    if (currentLineLength >= 40) {
      result += `<br/>`;
      currentLineLength = 0;
    }
    result += char;
  }

  return result;
}

// 修改对话框获取实时时间
// 定义一个格式化日期的方法
function formatDate(date: Date) {
  const year = date.getFullYear();
  const month = String(date.getMonth() + 1).padStart(2, "0");
  const day = String(date.getDate()).padStart(2, "0");
  const hours = String(date.getHours()).padStart(2, "0");
  const minutes = String(date.getMinutes()).padStart(2, "0");
  const seconds = String(date.getSeconds()).padStart(2, "0");

  return `${year}-${month}-${day} ${hours}:${minutes}:${seconds}`;
}
//首页挂载完毕 onMounted
onMounted(() => {
  // 更新时间的函数
  const updateCurrentTime = () => {
    formattedTime.value = formatDate(new Date());
  };
  // 初始设置时间
  updateCurrentTime();
  // 每秒更新时间
  setInterval(updateCurrentTime, 1000);
});

// 通知发布
async function issueNoticeData() {
  try {
    const response = await issueNoticeApi({
      phone: phone.value,
      title: form1.value.title,
      content: form1.value.content,
      createTime: formattedTime.value,
    });
    // console.log("通知发布: ", response.data);
    if (response.status == 1) {
      getNoticeData(currentPage.value);
      ElMessage({
        message: "发布成功",
        type: "success",
      });
      form1.value.title = "";
      form1.value.content = "";
    } else {
      ElMessage.error("发布失败");
    }
  } catch (error) {
    console.error("请求失败:", error);
  }
}
const handleSubmit = () => {
  issueNoticeData();
};
// 点击获取本行userId 数组
const handleSelectionChange = (val: any[]) => {
  // 数组获取选择的每行id
  selectedNoticeIds.value = val.map((item: any) => item.noticeId);
  if (selectedNoticeIds.value.length > 1) {
    length.value = true;
  } else {
    length.value = false;
  }
  console.log(selectedNoticeIds.value)
  console.log(selectedNoticeIds.value.length);
};
// 点击获取本行noticeId
const selectNotice = (row: any) => {
  // 数字形式
  selectedNoticeId.value = row.noticeId;
  form2.value.title = row.title;
  form2.value.content = row.content;
  // 数组形式
  if ((selectedNoticeIds.value = [])) {
    selectedNoticeIds.value.push(row.noticeId);
    console.log(selectedNoticeIds.value);
  }
};
// 通知修改
async function updataNoticeData() {
  try {
    const response = await updataNoticeApi({
      noticeId: selectedNoticeId.value,
      title: form2.value.title,
      content: form2.value.content,
      updateTime: formattedTime.value,
    });
    // console.log("通知修改: ", response.data);
    if (response.status == 1) {
      getNoticeData(currentPage.value);
      ElMessage({
        message: "修改成功",
        type: "success",
      });
    } else {
      ElMessage.error("修改失败");
    }
  } catch (error) {
    console.error("请求失败:", error);
  }
}
const updatenotice = () => {
  updataNoticeData();
};
// 通知删除
async function deleteNoticeData() {
  try {
    const response = await deleteNoticeApi({
      noticeIds: selectedNoticeIds.value,
    });
    // console.log("通知删除: ", response);
    if (response.status == 1) {
      getNoticeData(currentPage.value);
      ElMessage({
        message: "删除成功",
        type: "success",
      });
    } else {
      ElMessage.error("删除失败");
    }
  } catch (error) {
    console.error("请求失败:", error);
  }
}
const handledelete = () => {
  deleteNoticeData();
};
const btnDel = () => {
  deleteNoticeData();
};
</script>

<style scoped lang="scss">
.box {
  background: white;
  .right {
    height: 35px;
    margin-right: 10px;
    margin-bottom: 15px;
    float: right;
    line-height: 35px;
  }
  .elm-box {
    padding: 20px;
    .content-cell {
      white-space: nowrap; /* 防止文本换行 */
      overflow: hidden;
      text-overflow: ellipsis; /* 末尾添加省略号 */
      max-width: 100%; /* 限制单元格内容的最大宽度 */
    }
  }
  .page-box {
    height: 50px;
    .page {
      float: right;
      padding-right: 20px;
    }
  }
}
</style>

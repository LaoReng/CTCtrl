<template>
  <div class="user-management">
    <h1 class="page-title">用户管理</h1>
    <!-- lsw新增代码 -->
    <div class="search">

      <el-input placeholder="请输入内容" v-model="inputValue" class="input-with-select">
        <el-button slot="append" icon="el-icon-search"></el-button>
      </el-input>

      <el-button class="btn" type="primary" @click="open">新增用户</el-button>

    </div>
    <el-card style="border-radius: 15px" class="umg-box" shadow="always">
      <el-table :data="users" style="width: 100%">
        <el-table-column prop="id" label="用户ID" width="76"></el-table-column>
        <el-table-column prop="username" label="用户名" width="88" show-overflow-tooltip></el-table-column>
        <el-table-column prop="gender" label="性别" width="60"></el-table-column>
        <el-table-column prop="address" label="地址" show-overflow-tooltip></el-table-column>
        <el-table-column prop="phone" label="电话" show-overflow-tooltip></el-table-column>
        <el-table-column prop="registerTime" label="注册时间" width="150"></el-table-column>
        <el-table-column prop="balance" label="余额" show-overflow-tooltip></el-table-column>
        <el-table-column prop="vipStatus" label="VIP状态" width="71"></el-table-column>
        <el-table-column prop="vipLevel" label="VIP等级" width="71"></el-table-column>
        <el-table-column prop="totalRecharge" label="累计充值" show-overflow-tooltip></el-table-column>
        <el-table-column prop="expirationDate" label="到期时间" width="150"></el-table-column>
        <el-table-column prop="deviceControlTime" label="可控时间" width="76" show-overflow-tooltip></el-table-column>
        <el-table-column prop="controlCount" label="可控次数" width="76" show-overflow-tooltip></el-table-column>
        <el-table-column prop="remainingCount" label="剩余次数" width="76" show-overflow-tooltip></el-table-column>

        <!-- lsw新增的代码 -->
        <el-table-column fixed="right" label="操作" width="100">
          <template slot-scope="scope">
            <!-- 传递row.id就可以把当前的id传递给handleClick函数当中，从而进行数据库查找 -->
            <el-button @click="handleClick(scope.row.id)" type="text" size="small">查看</el-button>
            <el-button type="text" size="small">
              <el-popconfirm width="150px" @confirm="clickConfirmDelete(scope.row.id)" confirm-button-text='确定'
                cancel-button-text='取消' icon="el-icon-info" icon-color="red" :title="'确定删除' + scope.row.username + '吗'">
                <div slot="reference">删除</div>
              </el-popconfirm>

            </el-button>
          </template>
        </el-table-column>


      </el-table>
      <div class="block">
        <el-pagination @next-click="clickNext" @prev-click="clickPre" layout="prev, pager, next" :total="1000">
        </el-pagination>
      </div>


      <!-- 对话框 -->
      <el-dialog title="提示" :visible.sync="visible" width="30rem" :before-close="handleClose">
        <UserFormVue @showEvent="getVal" />
      </el-dialog>

      <!-- lsw新增结束 -->
    </el-card>
  </div>
</template>

<script>
import axios from "axios";
import UserFormVue from './UserForm.vue';
export default {
  data() {
    return {
      users: [
        {
          id: "user-001",
          username: "w",
          gender: "女",
          address: "黑龙江工程学院",
          phone: "12345678910",
          registerTime: "2024-04-05 20:13:14",
          balance: "$100.00",
          vipStatus: "激活",
          vipLevel: "1级",
          totalRecharge: "$500.00",
          expirationDate: "2024-5-05 20:13:14",
          deviceControlTime: "30天",
          controlCount: 100,
          remainingCount: 99,
        },
      ],
      // lsw新增开始
      inputValue: "",
      //是否弹出新增弹框
      visible: true,
      //lsw新增结束
    };
  },
  // async created() {
  //   try {
  //     const response = await axios.get("http://your-api-url/users");
  //     this.users = response.data;
  //   } catch (error) {
  //     console.error("Failed to fetch users:", error);
  //   }
  // },
  // lsw新增代码
  components: {
    UserFormVue
  },
  methods: {
    getVal(val) {
      this.visible = val
    },
    //在新增用关闭之前触发事件
    handleClose() {
      console.log("关闭事件触发");
      this.visible = false;
    },
    //新增用户
    open() {
      this.visible = true;
    },
    //点击上一个
    clickPre() {
      console.log("点击上一页");
    },
    //点击下一个
    clickNext() {
      console.log("点击下一页");
    },
    //点击查看
    handleClick(row) {
      console.log("您当前点击的角色id是：", row)
    },
    //点击编辑的触发按钮
    clickConfirmDelete(val) {
      // console.log("点击了");
      console.log("你要删除的用户id是", val);

    }
  }
};
</script>

<style scoped>
.user-management {
  padding: 20px;
}

/* lsw新增样式 */
.block {
  margin-top: 20px;
  text-align: right;
}

.search {
  width: 40rem;
  margin-bottom: 1rem;
  display: flex;

}

.search .btn {
  margin-left: 1rem;
}
</style>

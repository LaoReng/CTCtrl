<template>
  <div class="download-center">
    <div class="header">
      <p class="subtitle">选择您需要的被控端程序进行下载</p>
    </div>

    <el-table :data="downloadItems" style="width: 100%" stripe>
      <el-table-column label="程序" width="200">
        <template #default="{ row }">
          <div class="program-name" style="position: absolute;top: 3rem;right: 2rem;">
            <strong>{{ row.name }}</strong>
            <span class="version">{{ row.version }}</span>
          </div>
        </template>
      </el-table-column>
      <el-table-column label="简介" min-width="600">
        <template #default="{ row }">
          <div class="program-name">
            <p v-html="row.description"
              :style="isShow ? { 'font-size': '14px', 'user-select': 'none' } : { 'user-select': 'none', 'word-break': 'break-all', 'overflow': 'hidden', 'display': '-webkit-box', '-webkit-line-clamp': 2, '-webkit-box-orient': 'vertical' }">
            </p>
          </div>
          <div @click="seeMore" class="jiantou">
            <div class="bottom" v-if="!isShow">
              查看更多 <svg t="1713782986857" class="icon" viewBox="0 0 1024 1024" version="1.1"
                xmlns="http://www.w3.org/2000/svg" p-id="4316" width="12" height="12">
                <path d="M185.884 327.55 146.3 367.133 512.021 732.779 877.7 367.133 838.117 327.55 511.997 653.676Z"
                  p-id="4317"></path>
              </svg>

            </div>
            <div class="bottom" v-else>
              查看更多 <svg t="1713783053831" class="icon" viewBox="0 0 1024 1024" version="1.1"
                xmlns="http://www.w3.org/2000/svg" p-id="5313" width="12" height="12">
                <path d="M838.116 732.779 877.7 693.195 511.979 327.549 146.3 693.195 185.883 732.779 512.003 406.652Z"
                  p-id="5314"></path>
              </svg>

            </div>
          </div>
        </template>
      </el-table-column>
      <el-table-column label="操作" width="120">
        <template #default="{ row }">
          <el-button style="position: absolute;top: 3rem;right: 2rem;" type="primary" size="small" class="download-btn"
            @click="download(row)">
            下载
            <i class="el-icon-download"></i>
          </el-button>
        </template>
      </el-table-column>
    </el-table>
  </div>
</template>

<script>
export default {
  data() {
    return {
      isShow: false,
      "downloadItems": [
        {
          "name": "CTCtrl-beCtrl",
          "version": "v1.0.0",
          "description": "<p>被控端安装程序</p>\n<p>更新内容：</p>\n<li>支持用户登录</li>\n<li>支持设备被其他用户控制（这里没有加限制，全网公开）</li>\n<li>适配win托盘功能，支持Toast通知</li>\n<li>当前为软件测试阶段，请谨慎安装</li>\n</ul>",
          "downloadUrl": "http://81.70.91.154:9527/download/CTCtrl-beCtrl-V1.0.0.exe" // 替换为实际下载链接
        }
      ]
    }
  },
  methods: {
    seeMore() {
      this.isShow = !this.isShow;
    },
    download(item) {
      // window.open(item.downloadUrl);
      const a = document.createElement('a');
      a.href = item.downloadUrl;
      a.download = item.downloadUrl;
      a.click();
    },
  },
};
</script>

<style scoped>
.jiantou {
  position: absolute;
  top: 3rem;
  right: 9rem;
  user-select: none;

}

.download-center {
  padding: 3rem;
}

.header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-bottom: 20px;
  color: #333;
}

.header h1 {
  font-size: 24px;
  font-weight: bold;
}

.header .subtitle {
  font-size: 14px;
  color: #666;
}

.program-name {
  display: flex;
  align-items: baseline;
  gap: ½rem;
}

.program-name strong {
  font-size: 14px;
}

.program-name .version {
  font-size: 14px;
  color: #999;
}

.download-btn {
  white-space: nowrap;
}

.download-btn i {
  margin-left: ¾rem;
  vertical-align: middle;
}
</style>

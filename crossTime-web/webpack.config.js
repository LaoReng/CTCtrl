const path = require("path"); //nodejs核心模块，专门用来处理路径问题
const { VueLoaderPlugin } = require('vue-loader')
const HtmlWebpackPlugin = require('html-webpack-plugin');
const MiniCssExtractPlugin = require("mini-css-extract-plugin");


module.exports = {
    //1.入口
    entry: "./src/main.js", //相对路径
    //2.输出
    output: {
        //文件的输出路径
        //__dirname是node js 的变量 代表当前文件的文件夹的目录, 导出的文件名会放置的路径
        path: path.resolve(__dirname, "dist/"),
        //导出的文件名
        filename: "js/main.js",
        publicPath: "./",
        clean: true//自动将上次的打包内容清空
    },
    resolve: {
        alias: {
            // 指定使用vue包中的版本
            'vue$': 'vue/dist/vue.esm.js'
        }
    },
    //3.加载器
    module: {
        rules: [
            // 解析Vue文件
            {
                test: /\.vue$/,
                use: ['vue-loader'],
            },
            // 它会应用到普通的 `.js` 文件
            // 以及 `.vue` 文件中的 `<script>` 块
            {
                test: /\.m?js$/,
                //出去node_modules文件
                exclude: /(node_modules|bower_components)/,
                use: {
                    loader: 'babel-loader',
                    options: {
                        presets: ['@babel/preset-env'],
                    },
                },

            },
            // 它会应用到普通的 `.css` 文件
            // 以及 `.vue` 文件中的 `<style>` 块
            {
                test: /\.css$/i,
                use: [MiniCssExtractPlugin.loader, "css-loader"],
            },
            {
                test: /\.(png|jpe?g|git)$/,
                /* 数组对象形式 */
                use: [
                    {
                        // 使用file loader，解析文件路径
                        loader: "file-loader",
                        // 配置
                        options: {
                            outputPath: 'img/', // 如果需要指定输出目录
                            //这样的话就是user1/图片名字.后缀名
                            name: 'user1/[name].[ext]',
                        },

                    },
                ],
            },
            {
                test: /\.(jpg)$/,
                /* 数组对象形式 */
                use: [
                    {
                        // 使用file loader，解析文件路径
                        loader: "file-loader",
                        // 配置
                        options: {
                            outputPath: 'img/', // 如果需要指定输出目录
                            //这样的话就是user1/图片名字.后缀名
                            name: 'source/[name].[ext]',
                        },

                    },
                ],
            },
            // {
            //     test: /\.(woff2?|eot|ttf|otf)(\?.*)?$/,
            //     loader: 'url-loader',
            //     options: {
            //         // limit: 10000,
            //         name: utils.assetsPath('fonts/[name].[ext]')
            //     }
            // }

        ]
    },
    //4.插件
    plugins: [
        //plugin的配置
        new HtmlWebpackPlugin({
            template: './public/index.html',
            inject: true,
            // 定义环境变量
            env: {
                BASE_URL: process.env.BASE_URL || ''
            }
        }),
        new VueLoaderPlugin(),
        // 提取css成单独文件
        new MiniCssExtractPlugin({
            // 定义输出文件名和目录
            filename: "css/main.css",
        })


    ],
    //5.模式
    mode: "development",
};

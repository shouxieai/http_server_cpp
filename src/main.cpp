
#include "http_server.hpp"

using namespace std;

class LogicalController : public Controller{
	SetupController(LogicalController);
 
public: 
	DefRequestMapping(getCustom);
	DefRequestMapping(getReturn);
	DefRequestMapping(getBinary);
	DefRequestMapping(getFile);
	DefRequestMapping(putBase64Image);
};

Json::Value LogicalController::putBase64Image(const Json::Value& param){

	/**
	 * 注意，这个函数的调用，请用工具（postman）以提交body的方式(raw)提交base64数据
	 * 才能够在request.body中拿到对应的base64，并正确解码后储存
	 * 1. 可以在网页上提交一个图片文件，并使用postman进行body-raw提交，例如网址是：https://base64.us/，选择页面下面的“选择文件”按钮
	 * 2. 去掉生成的base64数据前缀：data:image/png;base64,。保证是纯base64数据输入
	 *   这是一个图像的base64案例：iVBORw0KGgoAAAANSUhEUgAAABwAAAAcCAYAAAByDd+UAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAABLSURBVEhLY2RY9OI/Ax0BE5SmG6DIh/8DJKAswoBxwwswTXcfjlpIdTBqIdXBqIVUB8O/8B61kOpg1EKqg1ELqQ5GLaQ6oLOFDAwA5z0K0dyTzgcAAAAASUVORK5CYII=
	 *   提交后能看到是个天蓝色的背景加上右上角有黄色的正方形
	 */

	auto session = get_current_session();
	auto image_data = iLogger::base64_decode(session->request.body);
	iLogger::save_file("base_decode.jpg", image_data);
	return success();
}

Json::Value LogicalController::getCustom(const Json::Value& param){

	auto session = get_current_session();
	const char* output = "hello http server";
	session->response.write_binary(output, strlen(output));
	session->response.set_header("Content-Type", "text/plain");
	return success();
}

Json::Value LogicalController::getReturn(const Json::Value& param){

	Json::Value data;
	data["alpha"] = 3.15;
	data["beta"] = 2.56;
	data["name"] = "张三";
	return success(data);
}

Json::Value LogicalController::getBinary(const Json::Value& param){

	auto session = get_current_session();
	auto data = iLogger::load_file("img.jpg");
	session->response.write_binary(data.data(), data.size());
	session->response.set_header("Content-Type", "image/jpeg");
	return success();
}

Json::Value LogicalController::getFile(const Json::Value& param){

	auto session = get_current_session();
	session->response.write_file("img.jpg");
	return success();
}

int test_http(int port = 8090){

	INFO("Create controller");
	auto logical_controller = make_shared<LogicalController>();
	string address = iLogger::format("0.0.0.0:%d", port);

	INFO("Create http server to: %s", address.c_str());
	auto server = createHttpServer(address, 32);
	if(!server)
		return -1;
 
	INFO("Add controller");
	server->add_controller("/v1", logical_controller);
	server->add_controller("/static", create_file_access_controller("./"));
	INFO("Access url: http://%s/v1", address.c_str());

	INFO(
		"\n"
		"访问如下地址即可看到效果:\n"
		"1. http://%s/v1/getCustom              使用自定义写出内容作为response\n"
		"2. http://%s/v1/getReturn              使用函数返回值中的json作为response\n"
		"3. http://%s/v1/getBinary              使用自定义写出二进制数据作为response\n"
		"4. http://%s/v1/getFile                使用自定义写出文件路径作为response\n"
		"5. http://%s/v1/putBase64Image         通过提交base64图像数据进行解码后储存\n"
		"6. http://%s/static/img.jpg            直接访问静态文件处理的controller，具体请看函数说明",
		address.c_str(), address.c_str(), address.c_str(), address.c_str(), address.c_str(), address.c_str()
	);

	INFO("按下Ctrl + C结束程序");
	return iLogger::while_loop();
}

int main(int argc, char** argv) {
	return test_http();
}

#include "http_server.hpp"

using namespace std;

class LogicalController : public Controller{
	SetupController(LogicalController);
 
public: 
	DefRequestMapping(getCustom);
	DefRequestMapping(getReturn);
	DefRequestMapping(getBinary);
	DefRequestMapping(getFile);
};

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
		"5. http://%s/static/img.jpg            直接访问静态文件处理的controller",
		address.c_str(), address.c_str(), address.c_str(), address.c_str(), address.c_str()
	);

	INFO("按下Ctrl + C结束程序");
	return while_loop();
}

int main(int argc, char** argv) {
	return test_http();
}
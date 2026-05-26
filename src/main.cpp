
int main() {
	vk::InstanceCreateInfo createinfo{};

	vk::Instance instance_ = vk::createInstance(createinfo);
	instance_.destroy();
	return 0;
}
# 2026-01-29T05:56:56.059575800
import vitis

client = vitis.create_client()
client.set_workspace(path="RISC_V_Code")

platform = client.create_platform_component(name = "platform123",hw_design = "$COMPONENT_LOCATION/../main_wrapper.xsa",os = "standalone",cpu = "microblaze_riscv_0",domain_name = "standalone_microblaze_riscv_0")

platform = client.create_platform_component(name = "platform_3",hw_design = "$COMPONENT_LOCATION/../main_wrapper.xsa",os = "standalone",cpu = "microblaze_riscv_0",domain_name = "standalone_microblaze_riscv_0")

platform = client.get_component(name="platform")
status = platform.build()

comp = client.get_component(name="hello_world")
comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

vitis.dispose()


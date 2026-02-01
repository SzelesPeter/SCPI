# 2026-02-01T11:19:17.739486200
import vitis

client = vitis.create_client()
client.set_workspace(path="RISC_V_Code")

platform = client.get_component(name="platform")
status = platform.build()

comp = client.get_component(name="hello_world")
comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

vitis.dispose()


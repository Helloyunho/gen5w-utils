from pathlib import Path
import subprocess
from typing import Optional
import tempfile


def odex_to_dex(filepath: Path, system_path: Path, out_path: Optional[Path] = None):
    if not filepath.exists() or filepath.suffix != ".odex":
        raise FileNotFoundError("Odex file not found.")
    if not system_path.exists():
        raise FileNotFoundError("System path not found.")
    out_path = out_path if out_path is not None else filepath.with_suffix(".dex")
    if not out_path.parent.exists():
        out_path.parent.mkdir(parents=True)
    with tempfile.TemporaryDirectory() as tmpdir:
        subprocess.run(  # TODO: automatically load every frameworks
            f"baksmali x -c am.jar:android-database-sqlcipher-3.4.0.jar:android.policy.jar:android.test.runner.jar:apache-xml.jar:automotive-service.jar:automotive.jar:bmgr.jar:bouncycastle.jar:bu.jar:com.android.future.usb.accessory.jar:com.android.location.provider.jar:com.android.media.remotedisplay.jar:com.broadcom.bt.jar:com.hkmc.ext.hcan.jar:com.infobank.jar:com.logfilter.jar:com.tcc.iap2.jar:com.telechips.android.tcciap.jar:conscrypt.jar:content.jar:core-junit.jar:core.jar:ext.jar:framework.jar:framework2.jar:ime.jar:input.jar:javax.obex.jar:media_cmd.jar:mms-common.jar:monkey.jar:okhttp.jar:pm.jar:requestsync.jar:services.jar:settings.jar:svc.jar:telephony-common.jar:uiautomator.jar:voip-common.jar:webviewchromium.jar:wm.jar:am.odex:android-database-sqlcipher-3.4.0.odex:android.policy.odex:android.test.runner.odex:apache-xml.odex:automotive-service.odex:automotive.odex:bmgr.odex:bouncycastle.odex:bu.odex:com.android.future.usb.accessory.odex:com.android.location.provider.odex:com.android.media.remotedisplay.odex:com.broadcom.bt.odex:com.hkmc.ext.hcan.odex:com.infobank.odex:com.logfilter.odex:com.tcc.iap2.odex:com.telechips.android.tcciap.odex:conscrypt.odex:content.odex:core-junit.odex:core.odex:ext.odex:framework.odex:framework2.odex:ime.odex:input.odex:javax.obex.odex:media_cmd.odex:mms-common.odex:monkey.odex:okhttp.odex:pm.odex:requestsync.odex:services.odex:settings.odex:svc.odex:telephony-common.odex:uiautomator.odex:voip-common.odex:webviewchromium.odex:wm.odex -d {system_path / 'framework'} -d {system_path / 'app'} -a 17 -o {tmpdir} {filepath}",
            shell=True,
            check=True,
        )
        subprocess.run(
            f"smali a -a 17 -o {out_path if out_path is not None else filepath.with_suffix('.dex')} {tmpdir}",
            shell=True,
            check=True,
        )


if __name__ == "__main__":
    import argparse

    ap = argparse.ArgumentParser()
    ap.add_argument("filepath", help="input file", type=Path)
    ap.add_argument(
        "-s",
        "--system-path",
        help="system path",
        type=Path,
        default=Path("system/"),
        dest="system_path",
    )
    ap.add_argument("-o", "--out", help="output file", type=Path, default=None)

    args = ap.parse_args()
    filepath: Path = args.filepath.resolve()
    system_path: Path = args.system_path.resolve()
    out_path = (
        args.out.resolve() if args.out is not None else filepath.with_suffix(".dex")
    )

    if filepath.exists() and system_path.exists():
        odex_to_dex(filepath, system_path, out_path)
